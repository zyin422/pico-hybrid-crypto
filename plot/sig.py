import matplotlib.pyplot as plt
import numpy as np

# File sizes
file_sizes = ['128 bits', '256 bits', '1024 bits', '4096 bits']
x = np.arange(len(file_sizes))
bar_width = 0.2
gap = 0.05

# Data in ms
rsa_sign = np.array([293164.8, 293157, 293146.2, 293134.8]) / 1000
mldsa_sign = np.array([41622.8, 45140.8, 88323.6, 75645]) / 1000
rsa_verify = np.array([8445, 8445.2, 8489.8, 8440]) / 1000
mldsa_verify = np.array([16765, 16761, 16862.4, 17242.8]) / 1000

fig, ax = plt.subplots(figsize=(12,6))

# Plot bars per file size in the order: RSA Sign, ML-DSA Sign, RSA Verify, ML-DSA Verify
bars1 = ax.bar(x - 1.5*bar_width - gap*1.5, rsa_sign, bar_width, label='RSA Sign', color='orange')
bars2 = ax.bar(x - 0.5*bar_width - gap*0.5, mldsa_sign, bar_width, label='ML-DSA Sign', color='skyblue')
bars3 = ax.bar(x + 0.5*bar_width + gap*0.5, rsa_verify, bar_width, label='RSA Verify', color='gold')
bars4 = ax.bar(x + 1.5*bar_width + gap*1.5, mldsa_verify, bar_width, label='ML-DSA Verify', color='deepskyblue')

# Labels and title
ax.set_ylabel('Time (ms)')
ax.set_title('RSA vs ML-DSA: Signing and Verification Times by File Size')
ax.set_xticks(x)
ax.set_xticklabels([f'{size}\n(n=5)' for size in file_sizes])

# Legend
ax.legend(loc='upper left', bbox_to_anchor=(1,1))

# Value labels
def add_labels(bars, fmt='.1f'):
    for bar in bars:
        h = bar.get_height()
        ax.annotate(f'{h:{fmt}}',
                    xy=(bar.get_x() + bar.get_width()/2, h),
                    xytext=(0,3),
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=9)

for bars in [bars1, bars2, bars3, bars4]:
    add_labels(bars)

plt.tight_layout()
plt.show()
