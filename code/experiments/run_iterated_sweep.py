#!/usr/bin/env python3

import csv
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "build" / "release" / "knapsack"
RESULTS = ROOT / "experiments" / "results"
OUT = RESULTS / "iteration_sweep.csv"

NS = [128, 256, 512, 1024, 2048, 4096]
SEEDS = [1, 2, 3]
REPS = 5
LAYER_VALUES = [2, 3, 4, 5]


def run(cmd, check=True):
    result = subprocess.run(
        cmd,
        cwd=ROOT,
        text=True,
        capture_output=True,
    )
    if check and result.returncode != 0:
        print("command failed:", " ".join(cmd))
        print(result.stdout)
        print(result.stderr)
        raise SystemExit(result.returncode)
    return result


def run_csv(args):
    result = run(args)
    rows = list(csv.DictReader(result.stdout.splitlines()))
    if len(rows) != 1:
        print(result.stdout)
        raise SystemExit("unexpected CSV output")
    return rows[0]


def run_config(layers):
    rows = []

    print("config", "layers", layers)

    for n in NS:
        for seed in SEEDS:
            print("bench", "mh-iterated", "layers", layers, "n", n, "seed", seed)
            row = run_csv(
                [
                    str(BIN),
                    "bench",
                    "--scheme",
                    "mh-iterated",
                    "--n",
                    str(n),
                    "--reps",
                    str(REPS),
                    "--layers",
                    str(layers),
                    "--seed",
                    str(seed),
                ]
            )
            if row["scheme"] != "mh-iterated":
                raise SystemExit("unexpected scheme in benchmark output")
            row["layers"] = str(layers)
            row["extra_layers"] = str(layers - 1)
            rows.append(row)

    return rows


def write_rows(rows):
    RESULTS.mkdir(parents=True, exist_ok=True)
    fieldnames = ["layers", "extra_layers"] + [
        key for key in rows[0].keys() if key not in {"layers", "extra_layers"}
    ]

    with OUT.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)

    print("wrote", OUT)


def main():
    rows = []

    run(["make", "release"])
    for layers in LAYER_VALUES:
        rows.extend(run_config(layers))

    write_rows(rows)


if __name__ == "__main__":
    main()
