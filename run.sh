#!/usr/bin/env bash

working_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

binary_file="$working_dir/build/turret/turret"
config_file="$working_dir/config/settings.json"

if [ -f "$binary_file" ]; then
    "$binary_file" "$config_file"
else
    echo "Project has not been compiled, please compile before running"
fi