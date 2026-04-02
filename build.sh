#!/bin/bash
# build.sh
CONFIG=${1:-Debug}
cmake --preset $(echo $CONFIG | tr '[:upper:]' '[:lower:]') && \
cmake --build --preset $(echo $CONFIG | tr '[:upper:]' '[:lower:]') && \
ln -sf Intermediate/$CONFIG/compile_commands.json compile_commands.json
