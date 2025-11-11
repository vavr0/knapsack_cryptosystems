#!/bin/bash
set -e
if [ "$1" = "super" ]; then
  ./build/super_knapsack "${2:-8}"
elif [ "$1" = "normal" ]; then
  ./build/normal_knapsack_bf "${2:-22}"   # when you add it
elif [ "$1" = "compare" ]; then
  ./build/compare
else
  ./build/knapsack   # default: your cryptosystem demo
fi
