#!/usr/bin/env python3

import csv
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "build" / "release" / "knapsack"
RESULTS = ROOT / "experiments" / "results"


def run_csv(args):
    result = subprocess.run(
        args,
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=True,
    )
    return list(csv.DictReader(result.stdout.splitlines()))[0]


def write_csv(name, rows):
    with (RESULTS / name).open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=rows[0].keys(), lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def run_crypto():
    schemes = ["mh-classic", "mh-permuted", "mh-iterated"]
    ns = [128, 256, 512, 1024, 2048, 4096]
    seeds = [1, 2, 3]
    reps = 5

    rows = []
    for scheme in schemes:
        for n in ns:
            for seed in seeds:
                print("bench", scheme, n, seed)
                rows.append(
                    run_csv(
                        [
                            str(BIN),
                            "bench",
                            "--scheme",
                            scheme,
                            "--n",
                            str(n),
                            "--reps",
                            str(reps),
                            "--seed",
                            str(seed),
                        ]
                    )
                )

    write_csv("crypto_bench.csv", rows)


def run_attacks():
    seeds = [1, 2, 3]
    attacks = [
        ("brute", [4, 6, 8, 10, 12, 14, 16, 18, 20, 22]),
        ("mitm", [4, 8, 12, 16, 20, 24, 28, 32, 36, 40]),
    ]

    rows = []
    for attack, ns in attacks:
        for n in ns:
            for seed in seeds:
                print("attack", attack, n, seed)
                rows.append(
                    run_csv(
                        [
                            str(BIN),
                            "attack",
                            "--attack",
                            attack,
                            "--n",
                            str(n),
                            "--seed",
                            str(seed),
                        ]
                    )
                )

    write_csv("attack_bench.csv", rows)


def main():
    RESULTS.mkdir(parents=True, exist_ok=True)
    run_crypto()
    run_attacks()


if __name__ == "__main__":
    main()
