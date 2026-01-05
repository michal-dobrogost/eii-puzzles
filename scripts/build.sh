#! /usr/bin/env nix-shell
#! nix-shell -i bash --pure build.nix
#
# The above shebang sets up the necessary packages to run this script without
# installing the packages globally on your computer. It's probably just CMake
# but you can check by reading the scripts/setup.nix file.
#
# If you have all the necessary tools already installed by a non-nix mechanism
# you can change to `#! /usr/bin/env bash` and run these scripts that way.
#

set -e
base=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )
BIN_PREFIX=~/exe
#BUILD_TYPE='Debug'
BUILD_TYPE='Release'

if [ ! -d "${BIN_PREFIX}" ]; then
  echo "ERROR: BIN_PREFIX directory does not exist: ${BIN_PREFIX}" >&2
  exit 1
fi

cmake --version
cmake -S src -B build -D CMAKE_BUILD_TYPE="${BUILD_TYPE}"
cmake --build build --config "${BUILD_TYPE}" --verbose
cmake --install build --config Release --verbose --prefix ${BIN_PREFIX}
