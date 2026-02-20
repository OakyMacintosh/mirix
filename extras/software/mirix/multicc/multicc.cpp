/* MIT License
 *
 * Copyright (c) 2026 The Mirix Operating System
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if __has_include(<unistd.h>)
#include <unistd.h>
#endif

#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sys/wait.h>
#include <vector>



static std::string trim(const std::string &s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::vector<std::string> split_tokens(const std::string &line, char sep = '|') {
    std::vector<std::string> result;
    std::string token;
    for (char ch : line) {
        if (ch == sep) {
            std::string t = trim(token);
            if (!t.empty()) {
                result.push_back(t);
            }
            token.clear();
        } else {
            token.push_back(ch);
        }
    }
    if (!token.empty()) {
        std::string t = trim(token);
        if (!t.empty()) {
            result.push_back(t);
        }
    }
    return result;
}

struct TargetDefinition {
    std::string name;
    std::string triple;
    std::string compiler = "clang";
    std::string linker = "clang";
    std::vector<std::string> compile_flags;
    std::vector<std::string> link_flags;
    std::vector<std::string> libs;
    std::string sysroot;
    std::string description;
};

static std::map<std::string, TargetDefinition> load_targets_from_file(const std::filesystem::path &path) {
    std::map<std::string, TargetDefinition> targets;
    if (!std::filesystem::exists(path)) {
        return targets;
    }

    std::ifstream input(path);
    if (!input) {
        std::cerr << "warning: cannot open targets file " << path << "\n";
        return targets;
    }

    std::string line;
    TargetDefinition current;
    bool in_section = false;

    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            if (in_section && !current.name.empty()) {
                targets[current.name] = current;
            }
            current = TargetDefinition();
            in_section = true;
            current.name = trim(line.substr(1, line.size() - 2));
            continue;
        }

        auto separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, separator));
        std::string value = trim(line.substr(separator + 1));

        if (key == "triple") {
            current.triple = value;
        } else if (key == "compiler") {
            current.compiler = value;
        } else if (key == "linker") {
            current.linker = value;
        } else if (key == "compile_flags") {
            current.compile_flags = split_tokens(value);
        } else if (key == "link_flags") {
            current.link_flags = split_tokens(value);
        } else if (key == "libs") {
            current.libs = split_tokens(value);
        } else if (key == "sysroot") {
            current.sysroot = value;
        } else if (key == "description") {
            current.description = value;
        }
    }

    if (in_section && !current.name.empty()) {
        targets[current.name] = current;
    }

    return targets;
}

static int run_command(const std::vector<std::string> &command) {
    if (command.empty()) {
        return -1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        std::vector<char *> args;
        args.reserve(command.size() + 1);
        for (const auto &part : command) {
            args.push_back(const_cast<char *>(part.c_str()));
        }
        args.push_back(nullptr);
        execvp(args[0], args.data());
        perror("execvp");
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

static TargetDefinition build_default_target() {
    TargetDefinition target;
    target.name = "mirix";
    target.triple = "any-none-aqua-lzyabi";
    target.compile_flags = {"-D__AQUA__=1", "-D__LZYABI__=1", "-Imirix/libc"};
    target.link_flags = {"-static", "-nostdlib", "-Lbuild/mirix/libc"};
    target.libs = {"-lmirixc"};
    target.description = "Default MIRIX lazykernel target";
    return target;
}

static std::filesystem::path default_targets_file(const std::filesystem::path &exe_path) {
    auto dir = exe_path.parent_path();
    auto candidate = dir / "targets.conf";
    if (std::filesystem::exists(candidate)) {
        return candidate;
    }
    candidate = dir / "../targets.conf";
    if (std::filesystem::exists(candidate)) {
        return std::filesystem::canonical(candidate);
    }
    return candidate;
}


void printHelpString() {
	// todo
}

int main(int argc, char **argv) {
    bool compile_only = false;
    std::string output = "a.out";
    std::string chosen_target;
    std::vector<std::string> inputs;
    std::vector<std::string> extra_args;
    std::filesystem::path targets_file;
    bool pass_extra = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (pass_extra) {
            extra_args.push_back(arg);
            continue;
        }

        if (arg == "-c") {
            compile_only = true;
            continue;
        }
        if (arg == "-o" && i + 1 < argc) {
            output = argv[++i];
            continue;
        }
        if (arg == "-t" && i + 1 < argc) {
            chosen_target = argv[++i];
            continue;
        }
        if (arg.rfind("--target=", 0) == 0) {
            chosen_target = arg.substr(9);
            continue;
        }
        if (arg == "--targets-file" && i + 1 < argc) {
            targets_file = argv[++i];
            continue;
        }
        if (arg == "--list-targets") {
            targets_file = "list";
            continue;
        }
        if (arg == "--") {
            pass_extra = true;
            continue;
        }
        if (!arg.empty() && arg[0] == '-') {
            extra_args.push_back(arg);
            continue;
        }
		if (arg == "--help") {
				printHelpString();
				continue;
		}
		if (arg == "-h") {
			printHelpString();
			continue;
		}
        inputs.push_back(arg);
    }

    std::filesystem::path exe_path = argv[0];
    if (exe_path.is_relative()) {
        exe_path = std::filesystem::current_path() / exe_path;
    }

    auto target_defs = load_targets_from_file(targets_file.empty() ? default_targets_file(exe_path) : targets_file);
    auto default_target = build_default_target();
    target_defs.emplace(default_target.name, default_target);

    if (targets_file == "list") {
        std::cout << "Available multicc targets:\n";
        for (const auto &[name, def] : target_defs) {
            std::cout << "  " << name << " - " << (def.description.empty() ? def.triple : def.description) << "\n";
        }
        return 0;
    }

    if (chosen_target.empty()) {
        chosen_target = default_target.name;
    }

    if (target_defs.find(chosen_target) == target_defs.end()) {
        std::cerr << "Unknown target: " << chosen_target << "\n";
        return 1;
    }

    const auto &target = target_defs[chosen_target];
    if (inputs.empty()) {
        std::cerr << "No input files provided.\n";
        return 1;
    }

    std::filesystem::path obj_dir = std::filesystem::current_path() / "build" / "multicc";
    std::filesystem::create_directories(obj_dir);

    std::vector<std::string> object_files;
    for (const auto &input : inputs) {
        std::filesystem::path src = input;
        if (!std::filesystem::exists(src)) {
            std::cerr << "Source file not found: " << src << "\n";
            return 1;
        }
        std::filesystem::path obj = obj_dir / (src.stem().string() + ".o");
        std::vector<std::string> compile_cmd;
        compile_cmd.push_back(target.compiler);
        compile_cmd.insert(compile_cmd.end(), target.compile_flags.begin(), target.compile_flags.end());
        compile_cmd.insert(compile_cmd.end(), extra_args.begin(), extra_args.end());
        compile_cmd.push_back("-c");
        compile_cmd.push_back(src.string());
        compile_cmd.push_back("-o");
        compile_cmd.push_back(obj.string());

        std::cout << "[multicc] compiling " << src << " -> " << obj << "\n";
        if (run_command(compile_cmd) != 0) {
            return 1;
        }
        object_files.push_back(obj.string());
    }

    if (compile_only) {
        return 0;
    }

    std::vector<std::string> link_cmd;
    link_cmd.push_back(target.linker);
    link_cmd.push_back("-target");
    link_cmd.push_back(target.triple);
    if (!target.sysroot.empty()) {
        link_cmd.push_back("--sysroot=" + target.sysroot);
    }
    link_cmd.push_back("-o");
    link_cmd.push_back(output);
    link_cmd.insert(link_cmd.end(), target.link_flags.begin(), target.link_flags.end());
    link_cmd.insert(link_cmd.end(), object_files.begin(), object_files.end());
    link_cmd.insert(link_cmd.end(), target.libs.begin(), target.libs.end());
    link_cmd.insert(link_cmd.end(), extra_args.begin(), extra_args.end());

    std::cout << "[multicc] linking -> " << output << "\n";
    if (run_command(link_cmd) != 0) {
        return 1;
    }

    return 0;
}
