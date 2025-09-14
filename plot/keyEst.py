import matplotlib.pyplot as plt
import numpy as np

# Average times in milliseconds (excluding Key Generation)
tasks = ['Key Sending/Encapsulation\n(n=5)', 'Key Receiving/Decapsulation\n(n=5)']
rsa_times = [8.1748, 0.1552]
mlkem_times = [4.3196, 6.2646]

x = np.arange(len(tasks))  # positions for groups
bar_width = 0.3
gap = 0.05  # spacing between bars in a group

fig, ax = plt.subplots(figsize=(10,6))

# Plot bars with spacing
bars1 = ax.bar(x - gap/2 - bar_width/2, rsa_times, bar_width, label='RSA', color='orange')
bars2 = ax.bar(x + gap/2 + bar_width/2, mlkem_times, bar_width, label='ML-KEM', color='skyblue')

# Linear scale (default)
# ax.set_yscale('log')  # removed

# Labels and title
ax.set_ylabel('Time (ms)')
ax.set_title('RSA vs ML-KEM Key Establishment: Sending & Receiving')
ax.set_xticks(x)
ax.set_xticklabels(tasks)
ax.legend()

# Add value labels on top of bars
def add_labels(bars):
    for bar in bars:
        height = bar.get_height()
        ax.annotate(f'{height:.2f}',
                    xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 3),
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=9)

add_labels(bars1)
add_labels(bars2)

plt.tight_layout()
plt.show()
