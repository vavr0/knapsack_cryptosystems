#!/usr/bin/env bash
set -euo pipefail

make san >/dev/null

BIN="./build/debug-asan/knapsack"

"$BIN" demo --scheme mh-classic --bits 101010 --seed 123 | grep -q "Status: OK"
"$BIN" demo --scheme mh-permuted --bits 101010 --seed 123 | grep -q "Status: OK"
"$BIN" demo --scheme mh-iterated --bits 101010 --seed 123 | grep -q "Status: OK"
"$BIN" demo --scheme mh-iterated --layers 3 --bits 101010 --seed 123 | grep -q "Status: OK"

"$BIN" demo --scheme mh-classic --msg "hello bachelor thesis knapsack test" --n 128 --seed 123 | grep -q "Decrypted-Text:"
"$BIN" demo --scheme mh-permuted --msg "hello bachelor thesis knapsack test" --n 128 --seed 123 | grep -q "Decrypted-Text:"
"$BIN" demo --scheme mh-iterated --msg "hello bachelor thesis knapsack test" --n 128 --seed 123 | grep -q "Decrypted-Text:"

"$BIN" bench --scheme mh-classic --n 256 --reps 1 --seed 123 | grep -q "mh-classic"
"$BIN" bench --scheme mh-permuted --n 256 --reps 1 --seed 123 | grep -q "mh-permuted"
"$BIN" bench --scheme mh-iterated --n 256 --reps 1 --seed 123 | grep -q "mh-iterated"
"$BIN" bench --scheme mh-iterated --layers 3 --n 256 --reps 1 --seed 123 | grep -q "mh-iterated"

"$BIN" attack --attack brute --n 12 --seed 1 | grep -q "brute,mh-classic,12"
"$BIN" attack --attack brute --bits 101010101010 --seed 1 | grep -q "brute,mh-classic,12"
"$BIN" attack --attack mitm --n 32 --seed 1 | grep -q "mitm,mh-classic,32"
"$BIN" attack --attack mitm --bits 10101010101010101010101010101010 --seed 1 | grep -q "mitm,mh-classic,32"

if "$BIN" attack --attack brute --n 31 --seed 1 >/dev/null 2>&1; then
    echo "expected brute n=31 to fail" >&2
    exit 1
fi

if "$BIN" attack --attack mitm --n 41 --seed 1 >/dev/null 2>&1; then
    echo "expected mitm n=41 to fail" >&2
    exit 1
fi

if "$BIN" attack --attack brute --scheme mh-permuted --n 8 >/dev/null 2>&1; then
    echo "expected permuted attack to fail" >&2
    exit 1
fi

if "$BIN" bench --scheme mh-classic --layers 3 --n 8 >/dev/null 2>&1; then
    echo "expected layers with mh-classic to fail" >&2
    exit 1
fi

if "$BIN" bench --scheme mh-iterated --layers 21 --n 8 >/dev/null 2>&1; then
    echo "expected layers=21 to fail" >&2
    exit 1
fi

echo "tests ok"
