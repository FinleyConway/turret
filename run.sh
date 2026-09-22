#!/usr/bin/env bash

build_type="$1"
working_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

binary_file="$working_dir/build/turret/turret"
config_file="$working_dir/config/settings.json"

if [ -f "$binary_file" ]; then
    echo "Running executable..."
    
    "$binary_file" "$config_file"
else
    echo "Project has not been compiled, compiling script..."

    if [ "$build_type" != "Release" ] && [ "$build_type" != "Debug" ]; then
        echo "No valid build type was included, building in Debug..."

        build_type="Debug"
    fi

    cmake -S "$working_dir" -B "$working_dir/build" -DCMAKE_BUILD_TYPE="$build_type"
    cmake --build "$working_dir/build"

    echo "Running executable..."

    "$binary_file" "$config_file"
fi