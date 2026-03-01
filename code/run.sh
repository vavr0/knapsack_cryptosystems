#!/bin/bash
set -euo pipefail

mode="${1:-demo}"
if [ "$#" -gt 0 ]; then
  shift
fi

make >/dev/null

case "$mode" in
  demo)
    ./build/knapsack demo "$@"
    ;;
  bench)
    ./build/knapsack bench "$@"
    ;;
  compare)
    ./build/knapsack bench --kind compare "$@"
    ;;
  -h|--help|help)
    echo "Usage: ./run.sh [demo|bench|compare] [args...]"
    ;;
  *)
    echo "Unknown mode: $mode" >&2
    echo "Usage: ./run.sh [demo|bench|compare] [args...]" >&2
    exit 1
    ;;
esac
