from pathlib import Path

import numpy as np


def hurst(prices):
    returns = np.log(prices[1:] / prices[:-1])
    n = len(returns)
    taus = np.unique(np.round(
        np.logspace(np.log10(10), np.log10(n // 4), 20)
    ).astype(int))

    rs_values = []
    valid_taus = []
    for window in taus:
        chunks = []
        for start in range(0, n - window + 1, window):
            chunk = returns[start:start + window]
            deviations = np.cumsum(chunk - np.mean(chunk))
            standard_deviation = np.std(chunk)
            if standard_deviation > 0:
                chunks.append(
                    (np.max(deviations) - np.min(deviations))
                    / standard_deviation
                )

        if chunks:
            rs_values.append(np.mean(chunks))
            valid_taus.append(window)

    H, intercept = np.polyfit(np.log(valid_taus), np.log(rs_values), 1)
    return H, np.exp(intercept)


csv_path = Path(__file__).with_name("spy.csv")
prices = np.loadtxt(csv_path, delimiter=",", skiprows=1, usecols=1)
H, c = hurst(prices)

print(f"H: {H}")
print(f"c: {c}")
