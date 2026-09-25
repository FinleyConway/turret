#!/usr/bin/env bash

# make script stop at external failures
set -e

working_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$working_dir"

turret_bin="./turret"
config_dir="./config/settings.json"

"$turret_bin" "$config_dir"