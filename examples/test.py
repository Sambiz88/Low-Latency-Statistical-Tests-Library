import time
import numpy as np

# --- R/S implementation (mirrors your C++ logic) ---
def compute_hurst(series):
    prices = np.array(series)
    # Step 1: log returns
    returns = np.log(prices[1:] / prices[:-1])
    n = len(returns)

    # Step 2: tau window sizes
    tau_min, tau_max, n_windows = 10, n // 4, 20
    taus = np.unique(np.round(
        np.logspace(np.log10(tau_min), np.log10(tau_max), n_windows)
    ).astype(int))

    # Step 3: R/S per window
    rs_vals = []
    clean_taus = []
    for w in taus:
        num_chunks = n // w
        if num_chunks == 0:
            continue
        rs_chunks = []
        for c in range(num_chunks):
            chunk = returns[c*w:(c+1)*w]
            mean  = np.mean(chunk)
            dev   = np.cumsum(chunk - mean)
            R     = np.max(dev) - np.min(dev)
            S     = np.std(chunk)
            if S > 0:
                rs_chunks.append(R / S)
        if rs_chunks:
            rs_vals.append(np.mean(rs_chunks))
            clean_taus.append(w)

    if len(clean_taus) < 2:
        return None, None

    # Step 4+5: log transform + OLS
    log_tau = np.log(clean_taus)
    log_rs  = np.log(rs_vals)
    H, intercept = np.polyfit(log_tau, log_rs, 1)
    c = np.exp(intercept)
    return H, c

# --- Helper: generate random walk ---
def generate_random_walk(n, seed=42):
    rng = np.random.default_rng(seed)
    returns = rng.normal(0.0, 1.0, n)
    return 100.0 * np.exp(np.cumsum(returns))

# --- Helper: measure in microseconds ---
def measure_once(series):
    start = time.perf_counter()
    H, c = compute_hurst(series)
    end   = time.perf_counter()
    if H is None:
        return None, None, None
    return (end - start) * 1e6, H, c

# -------------------------------------------------------
# Test 1: Scaling — runtime vs N
# -------------------------------------------------------
print("=== Test 1: Scaling (runtime vs N) ===")
print(f"{'N':<12}{'Mean(us)':<16}{'Min(us)':<16}{'Max(us)'}")

lengths = [500, 1000, 2000, 5000, 10000, 50000]
REPS = 10

for n in lengths:
    times = []
    for r in range(REPS):
        elapsed, _, _ = measure_once(generate_random_walk(n, seed=r))
        if elapsed is not None:
            times.append(elapsed)
    if times:
        print(f"{n:<12}{np.mean(times):<16.1f}{np.min(times):<16.1f}{np.max(times):.1f}")
    else:
        print(f"{n:<12}all failed")

# -------------------------------------------------------
# Test 2: Single series timing (N=5000)
# -------------------------------------------------------
print(f"\n=== Test 2: Single series timing (N=5000, {REPS} reps) ===")
series = generate_random_walk(5000)
times = [t for t, _, _ in (measure_once(series) for _ in range(REPS)) if t is not None]
print(f"Mean: {np.mean(times):.1f} us" if times else "All reps failed.")

# -------------------------------------------------------
# Test 3: Statistical bias — 1000 random walks
# -------------------------------------------------------
print("\n=== Test 3: Statistical bias (1000 random walks, N=1000) ===")
SIMS = 1000
Hvals = []

for r in range(SIMS):
    H, _ = compute_hurst(generate_random_walk(1000, seed=r))
    if H is not None and 0.0 < H < 1.0:
        Hvals.append(H)

Hvals = np.array(Hvals)
mean   = np.mean(Hvals)
stddev = np.std(Hvals)
bias   = mean - 0.5

print(f"Simulations:  {len(Hvals)} / {SIMS} valid")
print(f"Mean H:       {mean:.6f} (ideal: 0.5)")
print(f"Std H:        {stddev:.6f} (lower is better)")
print(f"Bias:         {bias:.6f} (ideal: 0.0)")

print("\nDone.")