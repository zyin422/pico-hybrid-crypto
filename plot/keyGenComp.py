import matplotlib.pyplot as plt
import numpy as np

# KeyGen times (all trials, µs)
rsa_trials = [36815985.2, 17245209.4, 18005361.4, 15760828.2, 19772458.8]
mlkem_trials = [4206.6]*5
mldsa_trials = [16073, 16005.4, 16090.4, 15979.8, 16073]

# Convert µs -> ms
rsa_trials_ms = [x / 1000 for x in rsa_trials]
mlkem_trials_ms = [x / 1000 for x in mlkem_trials]
mldsa_trials_ms = [x / 1000 for x in mldsa_trials]

# Compute averages
rsa_avg = np.mean(rsa_trials_ms)
mlkem_avg = np.mean(mlkem_trials_ms)
mldsa_avg = np.mean(mldsa_trials_ms)

# Values for plotting
algorithms = ['RSA\n(n=25)', 'ML-KEM\n(n=5)', 'ML-DSA\n(n=20)']  # added n=xx
values = [rsa_avg, mlkem_avg, mldsa_avg]
colors = ['orange', 'skyblue', 'lightgreen']

# Plot
fig, ax = plt.subplots(figsize=(8,6))
bars = ax.bar(algorithms, values, color=colors)

# Log scale
ax.set_yscale('log')
ax.set_ylabel('Time (ms, log scale)')
ax.set_title('Key Generation: RSA vs ML-KEM vs ML-DSA')

# Annotate bars
for bar, val in zip(bars, values):
    ax.annotate(f'{val:.2f} ms', xy=(bar.get_x() + bar.get_width()/2, val),
                xytext=(0,3), textcoords='offset points', ha='center', va='bottom', fontsize=10)

plt.tight_layout()
plt.show()
