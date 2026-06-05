#!/usr/bin/env python3

'''
Helper script to build HHVM dependencies via getdeps using a shared toolchain.
'''

import json
import os
import subprocess
import sys

# Build all Meta first-party deps, C dependencies for which no usable Ubuntu package exists
# and C++ dependencies (which should be built from source to use libc++).
# Order matters.
# We don't use `--recursive` to avoid accidentally picking up incompatible system libraries
# (if we were to set `--allow-system-packages`) or building a lot of unnecessary dependencies
# (if we were to not set `--allow-system-packages`).
HHVM_DEPENDENCIES = [
    "libunwind",
    "boost",
    "fast_float",
    "blake3",
    "fmt",
    "re2",
    "gflags",
    "glog",
    "magic_enum",
    "folly",
    "fizz",
    "wangle",
    "mvfst",
    "proxygen",
    "fbthrift",
    "mcrouter",
]

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GETDEPS = os.path.join(REPO_ROOT, "build", "fbcode_builder", "getdeps.py")
TOOLCHAIN = os.path.join(REPO_ROOT, "CMake", "HPHPClangToolchain.cmake")
# It would be nice to have this live under `_build/`, but getdeps' patching
# gets confused when run from within a Git repo.
# The next best thing is to have it adjacent to the HHVM checkout.
SCRATCH_PATH = os.path.join(os.path.dirname(REPO_ROOT), "hhvm-deps")


'''
Build a single project tracked by getdeps.
'''
def build_project(project: str):
    cmake_defines = {
        # Ensure nothing tries to build shared libraries (glog and gflags manifests are notably guilty of this).
        "BUILD_SHARED_LIBS": "Off",
        "BUILD_STATIC_LIBS": "On",
        # mcrouter doesn't have the standard getdeps modules mirrored to its repo, so we set it here.
        # This must also be set for all other projects we build because `extra-cmake-defines`
        # is factored into project install dir name calculations, so only setting it for mcrouter
        # causes it to not find transitive dependencies.
        "CMAKE_MODULE_PATH": os.path.join(REPO_ROOT, "build", "fbcode_builder", "CMake"),
        "CMAKE_TOOLCHAIN_FILE": TOOLCHAIN,
        "HPHP_ENABLE_HARDENING": "On",
        "HPHP_FORCE_LIBCPP": "On"
    }

    cmd = [
        sys.executable,
        GETDEPS,
        "build",
        "--no-deps",
        "--no-tests",
        "--extra-cmake-defines", json.dumps(cmake_defines),
        "--scratch-path", SCRATCH_PATH,
    ]

    if project == "boost":
        cmd += [
            "--extra-b2-args", "toolset=clang",
            "--extra-b2-args", "stdlib=libc++"
        ]

    cmd.append(project)

    print(f"Building {project}...")
    subprocess.check_call(cmd)


def main():
    for project in HHVM_DEPENDENCIES:
        build_project(project)


if __name__ == "__main__":
    main()
