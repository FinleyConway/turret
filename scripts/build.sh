#!/usr/bin/env bash

# make script stop at external failures
set -e

build_type="${1:-Debug}" # default to debug build
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
working_dir="$(cd "$script_dir/.." && pwd)"
pi_dir="pi@turret.local:/home/finley/"

check_build_type() {
    if [ "$build_type" != "Release" ] && [ "$build_type" != "Debug" ]; then
        echo "No valid build type was included, building in Debug..."

        build_type="Debug"
    fi
}

build_project() {
    cmake -S "$working_dir" -B "$working_dir/build" -DCMAKE_BUILD_TYPE="$build_type"
    cmake --build "$working_dir/build" -j"$(nproc)"
}

transfer_project() {
    scp -r "$working_dir/build/turret" "$pi_dir"
}

check_build_type
build_project
transfer_project