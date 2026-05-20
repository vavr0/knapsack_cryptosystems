#!/usr/bin/env python3

import csv
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src" / "scheme_mh_iterated.c"
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


def set_layer_count(layers):
    text = SOURCE.read_text()
    text, count = re.subn(
        r"#define MH_ITERATED_DEFAULT_LAYERS \d+u",
        f"#define MH_ITERATED_DEFAULT_LAYERS {layers}u",
        text,
    )
    if count != 1:
        raise SystemExit("could not update MH_ITERATED_DEFAULT_LAYERS")
    SOURCE.write_text(text)


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
    set_layer_count(layers)
    run(["make", "clean"])
    run(["make", "release"])

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


def restore_source_and_binary(original_source):
    SOURCE.write_text(original_source)
    run(["make", "clean"], check=False)
    result = run(["make", "release"], check=False)
    if result.returncode != 0:
        print("warning: restored source but could not rebuild default binary")
        print(result.stdout)
        print(result.stderr)


def main():
    original_source = SOURCE.read_text()
    rows = []

    try:
        for layers in LAYER_VALUES:
            rows.extend(run_config(layers))

        write_rows(rows)
    finally:
        restore_source_and_binary(original_source)


if __name__ == "__main__":
    main()
