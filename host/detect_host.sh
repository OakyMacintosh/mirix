#!/bin/bash

# Host detection script for MIRIX build system
# Outputs: OS-ARCH format (e.g., darwin-x86_64, linux-i386)

set -e

# Detect operating system
detect_os() {
    case "$(uname -s)" in
        Darwin)
            echo "darwin"
            ;;
        Linux)
            echo "linux"
            ;;
        FreeBSD)
            echo "freebsd"
            ;;
        NetBSD)
            echo "netbsd"
            ;;
        OpenBSD)
            echo "openbsd"
            ;;
        CYGWIN*|MINGW*|MSYS*)
            echo "winnt"
            ;;
        SunOS)
            echo "solaris"
            ;;
        AIX)
            echo "aix"
            ;;
        HP-UX)
            echo "hpux"
            ;;
        IRIX*)
            echo "irix"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

# Detect architecture
detect_arch() {
    case "$(uname -m)" in
        x86_64|amd64)
            echo "x86_64"
            ;;
        i386|i486|i586|i686)
            echo "i386"
            ;;
        arm64|aarch64)
            echo "arm64"
            ;;
        armv7l|armv6l)
            echo "arm"
            ;;
        mips64|mips64el)
            echo "mips64"
            ;;
        mips|mipsel)
            echo "mips"
            ;;
        powerpc|ppc)
            echo "powerpc"
            ;;
        powerpc64|ppc64)
            echo "powerpc64"
            ;;
        sparc64)
            echo "sparc64"
            ;;
        sparc)
            echo "sparc"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

# Main detection
OS=$(detect_os)
ARCH=$(detect_arch)

# Output in OS-ARCH format
echo "${OS}-${ARCH}"
