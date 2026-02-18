#!/usr/bin/awk -f
#
# Mirix Operating System
# (c) 2025-6 OakyMacintosh :3
#
# BSD Make reimplementation in AWK
# Supports: targets, dependencies, variables, pattern rules, .PHONY
#

BEGIN {
    # Configuration
    MAKEFILE = ENVIRON["MAKEFILE"] ? ENVIRON["MAKEFILE"] : "Makefile"
    if (!MAKEFILE && system("test -f makefile") == 0) MAKEFILE = "makefile"
    
    # State variables
    current_target = ""
    default_target = ""
    in_recipe = 0
    line_continuation = 0
    continued_line = ""
    
    # Track what we've built
    split("", built)
    split("", building)
    
    # Initialize arrays
    split("", targets)          # targets[name] = 1
    split("", deps)             # deps[target] = "dep1 dep2 ..."
    split("", recipes)          # recipes[target,linenum] = command
    split("", recipe_count)     # recipe_count[target] = number of lines
    split("", vars)             # vars[name] = value
    split("", phony)            # phony[target] = 1
    split("", pattern_targets)  # pattern_targets[pattern] = 1
    split("", pattern_deps)     # pattern_deps[pattern] = deps
    split("", pattern_recipes)  # pattern_recipes[pattern,linenum] = command
    split("", pattern_recipe_count) # pattern_recipe_count[pattern] = count
    
    # Special built-in variables
    vars["MAKE"] = "make"
    vars["CC"] = "cc"
    vars["CXX"] = "c++"
    vars["AR"] = "ar"
    vars["LD"] = "ld"
    vars["CFLAGS"] = ""
    vars["CXXFLAGS"] = ""
    vars["LDFLAGS"] = ""
}

# Read the makefile
{
    # Handle line continuations
    if (line_continuation) {
        continued_line = continued_line $0
        if (sub(/\\$/, "", continued_line)) {
            next
        }
        $0 = continued_line
        line_continuation = 0
        continued_line = ""
    } else if (/\\$/) {
        continued_line = $0
        sub(/\\$/, "", continued_line)
        line_continuation = 1
        next
    }
    
    # Skip comments and empty lines (but not recipe lines)
    if (!in_recipe && /^#/) next
    if (!in_recipe && /^[ \t]*$/) next
    
    # Variable assignment
    if (!in_recipe && $0 ~ /^[A-Za-z_][A-Za-z0-9_]*[[:space:]]*[?:]?=/) {
        # Parse variable assignment manually
        line_copy = $0
        
        # Extract variable name
        match(line_copy, /^[A-Za-z_][A-Za-z0-9_]*/)
        var_name = substr(line_copy, RSTART, RLENGTH)
        line_copy = substr(line_copy, RLENGTH + 1)
        
        # Skip whitespace
        sub(/^[[:space:]]*/, "", line_copy)
        
        # Extract operator
        if (line_copy ~ /^:=/) {
            op = ":="
            line_copy = substr(line_copy, 3)
        } else if (line_copy ~ /^\?=/) {
            op = "?="
            line_copy = substr(line_copy, 3)
        } else if (line_copy ~ /^=/) {
            op = "="
            line_copy = substr(line_copy, 2)
        }
        
        # Skip whitespace after operator
        sub(/^[[:space:]]*/, "", line_copy)
        value = line_copy
        
        # Expand variables in value
        value = expand_vars(value)
        
        if (op == "=" || op == ":=") {
            vars[var_name] = value
        } else if (op == "?=" && !(var_name in vars)) {
            vars[var_name] = value
        }
        next
    }
    
    # Recipe line (starts with tab)
    if (/^\t/) {
        if (current_target == "") {
            print "*** recipe line without target" > "/dev/stderr"
            exit 1
        }
        cmd = substr($0, 2)  # Remove leading tab
        
        if (current_target ~ /%/) {
            # Pattern rule recipe
            pattern_recipe_count[current_target]++
            pattern_recipes[current_target, pattern_recipe_count[current_target]] = cmd
        } else {
            recipe_count[current_target]++
            recipes[current_target, recipe_count[current_target]] = cmd
        }
        in_recipe = 1
        next
    }
    
    # Not a recipe line anymore
    in_recipe = 0
    
    # Special target .PHONY
    if (/^\.PHONY:/) {
        sub(/^\.PHONY:[[:space:]]*/, "")
        split($0, phony_targets)
        for (i in phony_targets) {
            phony[phony_targets[i]] = 1
        }
        current_target = ""
        next
    }
    
    # Target line with dependencies
    if (/:/) {
        # Split on first colon
        colon_pos = index($0, ":")
        target_part = substr($0, 1, colon_pos - 1)
        deps_part = substr($0, colon_pos + 1)
        
        # Remove leading/trailing whitespace
        gsub(/^[[:space:]]+|[[:space:]]+$/, "", target_part)
        gsub(/^[[:space:]]+|[[:space:]]+$/, "", deps_part)
        
        # Expand variables in target and deps
        target_part = expand_vars(target_part)
        deps_part = expand_vars(deps_part)
        
        # Handle multiple targets
        n = split(target_part, target_list)
        
        for (t = 1; t <= n; t++) {
            tgt = target_list[t]
            
            if (tgt == "") continue
            
            targets[tgt] = 1
            deps[tgt] = deps_part
            
            if (default_target == "" && tgt !~ /^\./) {
                default_target = tgt
            }
            
            current_target = tgt
            
            # Check if it's a pattern rule
            if (tgt ~ /%/) {
                pattern_targets[tgt] = 1
                pattern_deps[tgt] = deps_part
            }
        }
        next
    }
    
    # If we get here, reset current target
    current_target = ""
    next
}

# After reading makefile, build the target

END {
    # Determine what to build
    if (target != "") {
        build_target = target
    } else {
        build_target = default_target
    }
    
    if (build_target == "") {
        print "make: *** No targets specified and no makefile found.  Stop." > "/dev/stderr"
        exit 1
    }
    
    # Build the target
    exit_code = build(build_target)
    exit exit_code
}

# Expand variables in a string
function expand_vars(str,    result, pos, var_start, var_end, var_name, rest, paren_pos, brace_pos) {
    result = ""
    rest = str
    
    while (1) {
        # Look for $( or ${ or $X patterns
        paren_pos = index(rest, "$(")
        brace_pos = index(rest, "${")
        var_pos = match(rest, /\$[A-Za-z_<>@^]/)
        
        # Find the earliest match
        min_pos = 0
        match_type = 0
        
        if (paren_pos > 0 && (min_pos == 0 || paren_pos < min_pos)) {
            min_pos = paren_pos
            match_type = 1
        }
        if (brace_pos > 0 && (min_pos == 0 || brace_pos < min_pos)) {
            min_pos = brace_pos
            match_type = 2
        }
        if (var_pos > 0 && (min_pos == 0 || var_pos < min_pos)) {
            min_pos = var_pos
            match_type = 3
        }
        
        if (min_pos == 0) break
        
        # Add everything before the variable
        result = result substr(rest, 1, min_pos - 1)
        rest = substr(rest, min_pos)
        
        # Extract variable name based on match type
        if (match_type == 1) {
            # $(varname)
            close_pos = index(rest, ")")
            if (close_pos > 0) {
                var_name = substr(rest, 3, close_pos - 3)
                rest = substr(rest, close_pos + 1)
            } else {
                result = result "$("
                rest = substr(rest, 3)
                continue
            }
        } else if (match_type == 2) {
            # ${varname}
            close_pos = index(rest, "}")
            if (close_pos > 0) {
                var_name = substr(rest, 3, close_pos - 3)
                rest = substr(rest, close_pos + 1)
            } else {
                result = result "${"
                rest = substr(rest, 3)
                continue
            }
        } else {
            # $X or $@ etc
            match(rest, /\$[A-Za-z_<>@^]/)
            var_name = substr(rest, 2, RLENGTH - 1)
            rest = substr(rest, RLENGTH + 1)
        }
        
        # Special automatic variables
        if (var_name == "@") {
            result = result current_build_target
        } else if (var_name == "<") {
            result = result current_first_dep
        } else if (var_name == "^") {
            result = result current_all_deps
        } else if (var_name in vars) {
            result = result vars[var_name]
        }
    }
    
    result = result rest
    return result
}

# Check if a file exists and get its modification time
function file_mtime(file,    cmd, mtime) {
    # Use stat to get modification time
    cmd = "stat -c %Y \"" file "\" 2>/dev/null || stat -f %m \"" file "\" 2>/dev/null"
    cmd | getline mtime
    close(cmd)
    return mtime + 0
}

function file_exists(file) {
    return (system("test -f \"" file "\"") == 0)
}

# Check if target needs to be built
function needs_build(target, dep,    target_time, dep_time) {
    # Phony targets always need building
    if (target in phony) return 1
    
    # If target doesn't exist, needs building
    if (!file_exists(target)) return 1
    
    # Check dependencies
    if (!(target in deps) || deps[target] == "") return 0
    
    target_time = file_mtime(target)
    split(deps[target], dep_list)
    
    for (d in dep_list) {
        dep = dep_list[d]
        if (dep == "") continue
        
        # Build dependency first if needed
        if (build(dep) != 0) return -1  # Error in dependency
        
        dep_time = file_mtime(dep)
        if (dep_time > target_time) return 1
    }
    
    return 0
}

# Find a pattern rule that matches the target
function find_pattern_rule(target,    pattern, stem, dep, regex_pattern, percent_pos) {
    for (pattern in pattern_targets) {
        # Convert % to a regex pattern
        regex_pattern = pattern
        gsub(/\./, "\\.", regex_pattern)
        
        # Find the % position to extract the stem
        percent_pos = index(pattern, "%")
        if (percent_pos == 0) continue
        
        prefix = substr(pattern, 1, percent_pos - 1)
        suffix = substr(pattern, percent_pos + 1)
        
        # Check if target matches pattern
        if (length(target) >= length(prefix) + length(suffix)) {
            target_prefix = substr(target, 1, length(prefix))
            target_suffix = substr(target, length(target) - length(suffix) + 1)
            
            if (target_prefix == prefix && target_suffix == suffix) {
                stem = substr(target, length(prefix) + 1, length(target) - length(prefix) - length(suffix))
                return pattern "|" stem
            }
        }
    }
    return ""
}

# Build a target
function build(target,    result, i, cmd, dep_list, d, dep, exit_code, pattern_info, pattern, stem, rule_deps) {
    # Check for circular dependencies
    if (target in building) {
        print "make: Circular " target " <- " target " dependency dropped." > "/dev/stderr"
        return 0
    }
    
    # Already built?
    if (target in built) return 0
    
    building[target] = 1
    
    # Set automatic variables for this target
    current_build_target = target
    if (target in deps && deps[target] != "") {
        split(deps[target], dep_list)
        current_first_dep = dep_list[1]
        current_all_deps = deps[target]
    } else {
        current_first_dep = ""
        current_all_deps = ""
    }
    
    # Build dependencies first
    if (target in deps && deps[target] != "") {
        split(deps[target], dep_list)
        for (d in dep_list) {
            dep = dep_list[d]
            if (dep == "") continue
            
            exit_code = build(dep)
            if (exit_code != 0) {
                delete building[target]
                return exit_code
            }
        }
    }
    
    # Check if we need to build this target
    result = needs_build(target)
    
    if (result == -1) {
        delete building[target]
        return 1
    }
    
    # If target has no explicit rule, try pattern rules
    if (!(target in targets)) {
        pattern_info = find_pattern_rule(target)
        if (pattern_info != "") {
            split(pattern_info, parts, "|")
            pattern = parts[1]
            stem = parts[2]
            
            # Build pattern rule dependencies
            if (pattern in pattern_deps && pattern_deps[pattern] != "") {
                rule_deps = pattern_deps[pattern]
                gsub(/%/, stem, rule_deps)
                split(rule_deps, dep_list)
                
                for (d in dep_list) {
                    dep = dep_list[d]
                    if (dep == "") continue
                    
                    exit_code = build(dep)
                    if (exit_code != 0) {
                        delete building[target]
                        return exit_code
                    }
                }
                
                # Check if we need to build based on pattern rule dependencies
                if (!file_exists(target)) {
                    result = 1
                } else {
                    result = 0
                    target_time = file_mtime(target)
                    for (d in dep_list) {
                        dep = dep_list[d]
                        if (dep != "" && file_exists(dep)) {
                            dep_time = file_mtime(dep)
                            if (dep_time > target_time) {
                                result = 1
                                break
                            }
                        }
                    }
                }
            } else if (!file_exists(target)) {
                # Pattern rule with no deps, only build if target doesn't exist
                result = 1
            } else {
                result = 0
            }
            
            # Execute pattern rule recipe
            if (result != 0 && pattern in pattern_recipe_count) {
                # Set automatic variables for pattern rule
                current_build_target = target
                if (pattern in pattern_deps && pattern_deps[pattern] != "") {
                    rule_deps = pattern_deps[pattern]
                    gsub(/%/, stem, rule_deps)
                    split(rule_deps, dep_list)
                    current_first_dep = dep_list[1]
                    current_all_deps = rule_deps
                } else {
                    current_first_dep = ""
                    current_all_deps = ""
                }
                
                for (i = 1; i <= pattern_recipe_count[pattern]; i++) {
                    cmd = pattern_recipes[pattern, i]
                    gsub(/%/, stem, cmd)
                    cmd = expand_vars(cmd)
                    
                    if (cmd !~ /^@/) {
                        print cmd
                    } else {
                        cmd = substr(cmd, 2)
                    }
                    
                    exit_code = system(cmd)
                    if (exit_code != 0) {
                        print "make: *** [" target "] Error " exit_code > "/dev/stderr"
                        delete building[target]
                        return exit_code
                    }
                }
            }
            
            built[target] = 1
            delete building[target]
            return 0
        }
    }
    
    # If no recipe and target doesn't exist, error
    if (!(target in recipe_count) && !file_exists(target) && !(target in phony)) {
        print "make: *** No rule to make target '" target "'.  Stop." > "/dev/stderr"
        delete building[target]
        return 1
    }
    
    # Execute recipe if needed
    if (result != 0 && (target in recipe_count)) {
        # Reset automatic variables for this target's recipe
        current_build_target = target
        if (target in deps && deps[target] != "") {
            split(deps[target], dep_list)
            current_first_dep = dep_list[1]
            current_all_deps = deps[target]
        } else {
            current_first_dep = ""
            current_all_deps = ""
        }
        
        for (i = 1; i <= recipe_count[target]; i++) {
            cmd = recipes[target, i]
            cmd = expand_vars(cmd)
            
            # Handle @ prefix (silent)
            if (cmd !~ /^@/) {
                print cmd
            } else {
                cmd = substr(cmd, 2)
            }
            
            # Handle - prefix (ignore errors)
            ignore_errors = 0
            if (cmd ~ /^-/) {
                ignore_errors = 1
                cmd = substr(cmd, 2)
            }
            
            exit_code = system(cmd)
            if (exit_code != 0 && !ignore_errors) {
                print "make: *** [" target "] Error " exit_code > "/dev/stderr"
                delete building[target]
                return exit_code
            }
        }
    }
    
    built[target] = 1
    delete building[target]
    return 0
}
