#!/usr/bin/env python3

import csv
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "include" / "scheme_mh_common.h"
BIN = ROOT / "build" / "release" / "knapsack"
RESULTS = ROOT / "experiments" / "results"
OUT = RESULTS / "param_sweep.csv"

NS = [128, 256, 512, 1024, 2048, 4096]
SEEDS = [1, 2, 3]
REPS = 10

DELTA_VALUES = [1, 2, 4, 8, 16, 32, 64, 128]
MARGIN_VALUES = [1, 4, 16, 64, 256, 1024]


def run(cmd):
    result = subprocess.run(
        cmd,
        cwd=ROOT,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        print("command failed:", " ".join(cmd))
        print(result.stdout)
        print(result.stderr)
        raise SystemExit(result.returncode)
    return result


def set_constants(delta_max, margin_factor):
    text = HEADER.read_text()

    text, n_delta = re.subn(
        r"#define MH_DEFAULT_DELTA_MAX \d+u",
        f"#define MH_DEFAULT_DELTA_MAX {delta_max}u",
        text,
    )
    text, n_margin = re.subn(
        r"#define MH_DEFAULT_MARGIN_FACTOR \d+u",
        f"#define MH_DEFAULT_MARGIN_FACTOR {margin_factor}u",
        text,
    )

    if n_delta != 1 or n_margin != 1:
        raise SystemExit("could not update MH constants")

    HEADER.write_text(text)


def run_csv(args):
    result = run(args)
    rows = list(csv.DictReader(result.stdout.splitlines()))
    if len(rows) != 1:
        print(result.stdout)
        raise SystemExit("unexpected CSV output")
    return rows[0]


def run_config(sweep, delta_max, margin_factor):
    rows = []

    print("config", sweep, "delta", delta_max, "margin", margin_factor)
    set_constants(delta_max, margin_factor)
    run(["make", "clean"])
    run(["make", "release"])

    for n in NS:
        for seed in SEEDS:
            print("bench", sweep, "delta", delta_max, "margin", margin_factor, "n", n, "seed", seed)
            row = run_csv(
                [
                    str(BIN),
                    "bench",
                    "--scheme",
                    "mh-classic",
                    "--n",
                    str(n),
                    "--reps",
                    str(REPS),
                    "--seed",
                    str(seed),
                ]
            )
            if int(row["delta_max"]) != delta_max:
                raise SystemExit("built binary used wrong delta_max")
            if int(row["margin_bound"]) != margin_factor * n:
                raise SystemExit("built binary used wrong margin_bound")
            row["sweep"] = sweep
            rows.append(row)

    return rows


def write_rows(rows):
    RESULTS.mkdir(parents=True, exist_ok=True)
    fieldnames = ["sweep"] + [key for key in rows[0].keys() if key != "sweep"]

    with OUT.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)

    print("wrote", OUT)


def main():
    original_header = HEADER.read_text()
    rows = []

    try:
        for delta_max in DELTA_VALUES:
            rows.extend(run_config("delta", delta_max, 64))

        for margin_factor in MARGIN_VALUES:
            rows.extend(run_config("margin", 16, margin_factor))

        write_rows(rows)
    finally:
        HEADER.write_text(original_header)


if __name__ == "__main__":
    main()
