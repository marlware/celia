#!/usr/bin/env python3
"""Plot Celia benchmark results as execution-time-vs-matrix-size line charts."""

import argparse
import csv
from collections import defaultdict

import matplotlib.pyplot as plt


def load_results(csv_path):
    by_threads = defaultdict(list)
    with open(csv_path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            threads = int(row["threads"])
            size = int(row["matrix_size"])
            seconds = float(row["seconds"])
            by_threads[threads].append((size, seconds))

    for threads in by_threads:
        by_threads[threads].sort(key=lambda point: point[0])

    return by_threads


def plot_results(by_threads, output_path):
    fig, ax = plt.subplots(figsize=(8, 5))

    for threads in sorted(by_threads):
        sizes, seconds = zip(*by_threads[threads])
        ax.plot(sizes, seconds, marker="o", label=f"{threads} thread{'s' if threads != 1 else ''}")

    ax.set_xlabel("Matrix size (N x N)")
    ax.set_ylabel("Execution time (s)")
    ax.set_title("Celia: matrix multiply time vs. size and thread count")
    ax.legend(title="Threads")
    ax.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(output_path, dpi=150)
    print(f"Wrote plot to {output_path}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv_path", help="Path to a results CSV produced by the celia binary")
    parser.add_argument("-o", "--output", default="results.png", help="Output image path")
    args = parser.parse_args()

    by_threads = load_results(args.csv_path)
    plot_results(by_threads, args.output)


if __name__ == "__main__":
    main()
