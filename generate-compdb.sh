#!/usr/bin/env bash
set -e

# Generate a database for each assignment
while IFS= read -r -d '' makefile; do
    dir=$(dirname "$makefile")
    echo "Processing: $dir"
    (cd "$dir" && compiledb -n make)
done < <(find . -name Makefile -print0)

# Merge the individual databases
find . -mindepth 2 -name compile_commands.json \
    -exec jq -s 'add' {} + > compile_commands.json
