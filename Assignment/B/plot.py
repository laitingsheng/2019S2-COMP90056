#!/usr/bin/env python3

import sys

import numpy as np
import matplotlib.pyplot as plt


def split(line):
    v, record, sample = line.split()
    return int(v), int(record), int(sample)


def plot(x, ry, sy, loc, index, name):
    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)
    ax1.bar(x, ry)
    ax2.bar(x, sy)
    fig.savefig(f"{loc}/1-{name}.jpg")


def main(argv):
    with open(argv[1]) as f:
        data = np.array(list(split(line) for line in (line.strip() for line in f) if line), np.uint64)
        hl = len(data) // 2
        uniform = data[:hl]
        plot(uniform[:, 0].flatten(),
             uniform[:, 1].flatten(),
             uniform[:, 2].flatten(),
             argv[2],
             argv[3],
             "Uniform")

        binomial = data[hl:]
        plot(binomial[:, 0].flatten(),
             binomial[:, 1].flatten(),
             binomial[:, 2].flatten(),
             argv[2],
             argv[3],
             "Binomial")


if __name__ == "__main__":
    main(sys.argv)
