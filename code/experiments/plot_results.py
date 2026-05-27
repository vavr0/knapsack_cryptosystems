#!/usr/bin/env python3

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
RESULTS = ROOT / "experiments" / "results"
PLOTS = ROOT / "experiments" / "plots"


def plot_crypto_total(df):
    avg = df.groupby(["scheme", "n"], as_index=False)["total_ms"].mean()

    for scheme in ["mh-classic", "mh-permuted", "mh-iterated"]:
        rows = avg[avg["scheme"] == scheme].sort_values("n")
        if rows.empty:
            continue
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


def plot_crypto_keygen_by_scheme(df):
    avg = df.groupby(["scheme", "n"], as_index=False)["keygen_ms"].mean()

    for scheme in avg["scheme"].unique():
        rows = avg[avg["scheme"] == scheme].sort_values("n")
        plt.plot(rows["n"], rows["keygen_ms"], marker="o", label=scheme)

    plt.title("Merkle-Hellman key generation time")
    plt.xlabel("block size n (bits)")
    plt.ylabel("average key generation time (ms)")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out = PLOTS / "crypto_keygen_by_scheme.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_crypto_density_by_scheme(df):
    avg = df.groupby(["scheme", "n"], as_index=False)["density"].mean()

    for scheme in avg["scheme"].unique():
        rows = avg[avg["scheme"] == scheme].sort_values("n")
        plt.plot(rows["n"], rows["density"], marker="o", label=scheme)

    y_min = max(0.0, avg["density"].min() - 0.01)
    y_max = min(1.05, avg["density"].max() + 0.01)
    plt.ylim(y_min, y_max)
    plt.title("Public knapsack density by scheme")
    plt.xlabel("block size n (bits)")
    plt.ylabel("density")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out = PLOTS / "crypto_density_by_scheme.png"
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


def plot_crypto_components_by_scheme(df):
    schemes = ["mh-classic", "mh-permuted", "mh-iterated"]
    fig, axes = plt.subplots(1, len(schemes), figsize=(13, 4), sharey=True)

    for ax, scheme in zip(axes, schemes):
        rows = df[df["scheme"] == scheme]
        avg = rows.groupby("n", as_index=False)[
            ["keygen_ms", "encrypt_ms", "decrypt_ms"]
        ].mean()

        ax.plot(avg["n"], avg["keygen_ms"], marker="o", label="key generation")
        ax.plot(avg["n"], avg["encrypt_ms"], marker="o", label="encryption")
        ax.plot(avg["n"], avg["decrypt_ms"], marker="o", label="decryption")
        ax.set_title(scheme)
        ax.set_xlabel("block size n (bits)")
        ax.set_xscale("log", base=2)
        ax.grid(True, alpha=0.3)

    axes[0].set_ylabel("average time (ms)")
    axes[-1].legend()
    fig.suptitle("Merkle-Hellman operation breakdown by scheme")
    fig.tight_layout()

    out = PLOTS / "crypto_components_by_scheme.png"
    fig.savefig(out, dpi=200)
    plt.close(fig)
    print("wrote", out)


def plot_classic_sum_bits(df):
    classic = df[df["scheme"] == "mh-classic"]
    avg = classic.groupby("n", as_index=False)["sum_bits"].mean()

    plt.plot(avg["n"], avg["sum_bits"], marker="o")
    plt.title("Private superincreasing sum growth")
    plt.xlabel("block size n (bits)")
    plt.ylabel("bit length of sum(W)")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    out = PLOTS / "classic_sum_bits.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_classic_density(df):
    classic = df[df["scheme"] == "mh-classic"]
    avg = classic.groupby("n", as_index=False)["density"].mean()

    plt.plot(avg["n"], avg["density"], marker="o")
    plt.title("Public knapsack density")
    plt.xlabel("block size n (bits)")
    plt.ylabel("density")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    out = PLOTS / "classic_density.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_classic_margin(df):
    classic = df[df["scheme"] == "mh-classic"]
    avg = classic.groupby("n", as_index=False)[["margin", "margin_bound"]].mean()

    plt.plot(avg["n"], avg["margin_bound"], marker="o", label="margin bound")
    plt.plot(avg["n"], avg["margin"], marker="o", label="chosen margin")
    plt.title("Modulus margin selection")
    plt.xlabel("block size n (bits)")
    plt.ylabel("margin")
    plt.xscale("log", base=2)
    plt.yscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out = PLOTS / "classic_margin.png"
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


def plot_delta_sweep_density(df):
    rows = df[df["sweep"] == "delta"]
    avg = rows.groupby(["delta_max", "n"], as_index=False)["density"].mean()

    for delta_max in sorted(avg["delta_max"].unique()):
        group = avg[avg["delta_max"] == delta_max].sort_values("n")
        plt.plot(group["n"], group["density"], marker="o", label=f"Δ={delta_max}")

    plt.title("Effect of superincreasing increment on density")
    plt.xlabel("block size n (bits)")
    plt.ylabel("density")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend(title="delta max")
    plt.tight_layout()

    out = PLOTS / "sweep_delta_density.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_delta_sweep_sum_bits(df):
    rows = df[df["sweep"] == "delta"]
    avg = rows.groupby(["delta_max", "n"], as_index=False)["sum_bits"].mean()

    for delta_max in sorted(avg["delta_max"].unique()):
        group = avg[avg["delta_max"] == delta_max].sort_values("n")
        plt.plot(group["n"], group["sum_bits"], marker="o", label=f"Δ={delta_max}")

    plt.title("Effect of superincreasing increment on private sum size")
    plt.xlabel("block size n (bits)")
    plt.ylabel("bit length of sum(W)")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend(title="delta max")
    plt.tight_layout()

    out = PLOTS / "sweep_delta_sum_bits.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_margin_sweep_margin(df):
    rows = df[df["sweep"] == "margin"].copy()
    rows["margin_factor"] = (rows["margin_bound"] / rows["n"]).round().astype(int)
    avg = rows.groupby(["margin_factor", "n"], as_index=False)["margin"].mean()

    for margin_factor in sorted(avg["margin_factor"].unique()):
        group = avg[avg["margin_factor"] == margin_factor].sort_values("n")
        plt.plot(group["n"], group["margin"], marker="o", label=f"F={margin_factor}")

    plt.title("Effect of margin bound on selected modulus margin")
    plt.xlabel("block size n (bits)")
    plt.ylabel("average selected margin")
    plt.xscale("log", base=2)
    plt.yscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend(title="margin factor")
    plt.tight_layout()

    out = PLOTS / "sweep_margin_selected.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_margin_sweep_density(df):
    rows = df[df["sweep"] == "margin"].copy()
    rows["margin_factor"] = (rows["margin_bound"] / rows["n"]).round().astype(int)
    avg = rows.groupby(["margin_factor", "n"], as_index=False)["density"].mean()

    for margin_factor in sorted(avg["margin_factor"].unique()):
        group = avg[avg["margin_factor"] == margin_factor].sort_values("n")
        plt.plot(group["n"], group["density"], marker="o", label=f"F={margin_factor}")

    plt.title("Effect of margin bound on density")
    plt.xlabel("block size n (bits)")
    plt.ylabel("density")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend(title="margin factor")
    plt.tight_layout()

    out = PLOTS / "sweep_margin_density.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)


def plot_iterated_layers_total(df):
    rows = df.copy()
    rows["layers"] = rows["layers"].astype(int)
    avg = rows.groupby(["layers", "n"], as_index=False)["total_ms"].mean()

    for layers in sorted(avg["layers"].unique()):
        group = avg[avg["layers"] == layers].sort_values("n")
        plt.plot(group["n"], group["total_ms"], marker="o", label=f"{layers} layers")

    plt.title("Runtime of iterated variant by layer count")
    plt.xlabel("block size n (bits)")
    plt.ylabel("average total time (ms)")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend(title="total layers")
    plt.tight_layout()

    out = PLOTS / "iterated_layers_total_ms.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)



def plot_iterated_layers_density(df):
    rows = df.copy()
    rows["layers"] = rows["layers"].astype(int)
    avg = rows.groupby(["layers", "n"], as_index=False)["density"].mean()

    for layers in sorted(avg["layers"].unique()):
        group = avg[avg["layers"] == layers].sort_values("n")
        plt.plot(group["n"], group["density"], marker="o", label=f"{layers} layers")

    y_min = max(0.0, avg["density"].min() - 0.01)
    y_max = min(1.05, avg["density"].max() + 0.01)
    plt.ylim(y_min, y_max)
    plt.title("Density of iterated variant by layer count")
    plt.xlabel("block size n (bits)")
    plt.ylabel("density")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend(title="total layers")
    plt.tight_layout()

    out = PLOTS / "iterated_layers_density.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)



def plot_crypto_total_with_iterated_layers(df, layer_df):
    avg = df.groupby(["scheme", "n"], as_index=False)["total_ms"].mean()

    for scheme in ["mh-classic", "mh-permuted", "mh-iterated"]:
        rows = avg[avg["scheme"] == scheme].sort_values("n")
        if rows.empty:
            continue
        label = "mh-iterated (2 layers)" if scheme == "mh-iterated" else scheme
        plt.plot(rows["n"], rows["total_ms"], marker="o", label=label)

    layer_rows = layer_df.copy()
    layer_rows["layers"] = layer_rows["layers"].astype(int)
    layer_avg = layer_rows.groupby(["layers", "n"], as_index=False)["total_ms"].mean()
    for layers in sorted(layer_avg["layers"].unique()):
        if layers == 2:
            continue
        group = layer_avg[layer_avg["layers"] == layers].sort_values("n")
        plt.plot(
            group["n"],
            group["total_ms"],
            marker="o",
            linestyle="--",
            label=f"mh-iterated ({layers} layers)",
        )

    plt.title("Runtime with additional iterated layers")
    plt.xlabel("block size n (bits)")
    plt.ylabel("average total time (ms)")
    plt.xscale("log", base=2)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    out = PLOTS / "crypto_total_with_iterated_layers.png"
    plt.savefig(out, dpi=200)
    plt.close()
    print("wrote", out)



def main():
    PLOTS.mkdir(parents=True, exist_ok=True)

    df = pd.read_csv(RESULTS / "crypto_bench.csv")
    plot_crypto_total(df)
    plot_crypto_keygen_by_scheme(df)
    plot_crypto_density_by_scheme(df)
    plot_crypto_components(df)
    plot_crypto_components_by_scheme(df)
    plot_classic_sum_bits(df)
    plot_classic_density(df)
    plot_classic_margin(df)

    sweep_path = RESULTS / "param_sweep.csv"
    if sweep_path.exists():
        sweep = pd.read_csv(sweep_path)
        plot_delta_sweep_density(sweep)
        plot_delta_sweep_sum_bits(sweep)
        plot_margin_sweep_margin(sweep)
        plot_margin_sweep_density(sweep)

    iteration_path = RESULTS / "iteration_sweep.csv"
    if iteration_path.exists():
        iterations = pd.read_csv(iteration_path)
        plot_iterated_layers_total(iterations)
        plot_iterated_layers_density(iterations)
        plot_crypto_total_with_iterated_layers(df, iterations)

    attack_path = RESULTS / "attack_bench.csv"
    if attack_path.exists():
        attacks = pd.read_csv(attack_path)
        plot_attack_time(attacks)
        plot_mitm_table_entries(attacks)


if __name__ == "__main__":
    main()
