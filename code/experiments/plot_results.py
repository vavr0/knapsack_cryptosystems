#!/usr/bin/env python3

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
RESULTS = ROOT / "experiments" / "results"
PLOTS = ROOT / "experiments" / "plots"


def plot_crypto_total(df):
    avg = df.groupby(["scheme", "n"], as_index=False)["total_ms"].mean()

    for scheme in avg["scheme"].unique():
        rows = avg[avg["scheme"] == scheme].sort_values("n")
        plt.plot(rows["n"], rows["total_ms"], marker="o", label=scheme)

    plt.title("Merkle-Hellman variant runtime")
    plt.xlabel("block size n (bits)")
    plt.ylabel("average total time (ms)")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out = PLOTS / "crypto_total_ms.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_crypto_components(df):
    classic = df[df["scheme"] == "mh-classic"]
    avg = classic.groupby("n", as_index=False)[
        ["keygen_ms", "encrypt_ms", "decrypt_ms"]
    ].mean()

    plt.plot(avg["n"], avg["keygen_ms"], marker="o", label="key generation")
    plt.plot(avg["n"], avg["encrypt_ms"], marker="o", label="encryption")
    plt.plot(avg["n"], avg["decrypt_ms"], marker="o", label="decryption")

    plt.title("Classic Merkle-Hellman operation breakdown")
    plt.xlabel("block size n (bits)")
    plt.ylabel("average time (ms)")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out = PLOTS / "crypto_components_classic.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_attack_time(df):
    avg = df.groupby(["attack", "n"], as_index=False)["attack_ms"].mean()

    for attack in avg["attack"].unique():
        rows = avg[avg["attack"] == attack].sort_values("n")
        plt.plot(rows["n"], rows["attack_ms"], marker="o", label=attack)

    plt.title("Subset-sum attack runtime")
    plt.xlabel("message length n (bits)")
    plt.ylabel("average attack time (ms)")
    plt.yscale("log")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out = PLOTS / "attack_ms.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_mitm_table_entries(df):
    mitm = df[df["attack"] == "mitm"]
    avg = mitm.groupby("n", as_index=False)["table_entries"].mean()

    plt.plot(avg["n"], avg["table_entries"], marker="o")
    plt.title("Meet-in-the-middle table size")
    plt.xlabel("message length n (bits)")
    plt.ylabel("table entries")
    plt.yscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    out = PLOTS / "mitm_table_entries.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def main():
    PLOTS.mkdir(parents=True, exist_ok=True)

    df = pd.read_csv(RESULTS / "crypto_bench.csv")
    plot_crypto_total(df)
    plot_crypto_components(df)

    attack_path = RESULTS / "attack_bench.csv"
    if attack_path.exists():
        attacks = pd.read_csv(attack_path)
        plot_attack_time(attacks)
        plot_mitm_table_entries(attacks)


if __name__ == "__main__":
    main()
