#!/bin/bash
current_dir=$(dirname $(readlink -f $0))
cd "$current_dir/.."

# Run code
./build/exe/simulation.exe
