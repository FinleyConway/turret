#!/usr/bin/env bash

# make script stop at external failures
set -e

build_type="${1:-Debug}" # default to debug build
platform_type="${2:-Pi}" # default to pi
working_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

binary_file="$working_dir/build/turret/turret"
config_file="$working_dir/config/settings.json"

# make sure we are working inside the project directory
cd "$working_dir"

build_project() {
    cmake -S "$working_dir" -B "$working_dir/build" -DCMAKE_BUILD_TYPE="$build_type"

    if [ "$platform_type" == "Pi" ]; then
        cmake --build "$working_dir/build" --parallel 1 --verbose
    else
        cmake --build "$working_dir/build"
    fi
}

check_build_type() {
    if [ "$build_type" != "Release" ] && [ "$build_type" != "Debug" ]; then
        echo "No valid build type was included, building in Debug..."

        build_type="Debug"
    fi
}

check_update_and_build() {
    echo "Checking for updates..."

    git fetch

    local_hash="$(git rev-parse HEAD)" 
    remote_hash="$(git rev-parse '@{u}')"

    if [ "$local_hash" != "$remote_hash" ]; then
        echo "Repository has changed, pulling updates..."
    
        git merge --ff-only '@{u}'

        echo "Building new changes..."

        build_project
    else
        echo "Up to date..."

        if [ ! -f "$binary_file" ]; then
            echo "Binary doesn't exist, building..."

            build_project
        fi
    fi
}

check_build_type
check_update_and_build

echo "Starting turret..."

"$binary_file" "$config_file"