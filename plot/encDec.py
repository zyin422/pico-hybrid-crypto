import matplotlib.pyplot as plt
import numpy as np

# Groups: file size + operation
groups = ['128 bits Enc', '128 bits Dec', '256 bits Enc', '256 bits Dec', 
          '1024 bits Enc', '1024 bits Dec', '4096 bits Enc', '4096 bits Dec']

# AES-128 values (average in µs)
aes128_us = [
    np.mean([448, 359, 289, 304, 314]),
    np.mean([340, 286, 293, 354, 329]),
    np.mean([441, 403, 298, 280, 304]),
    np.mean([348, 315, 345, 326, 347]),
    np.mean([770, 568, 530, 544, 538]),
    np.mean([659, 594, 575, 568, 601]),
    np.mean([1130, 1026, 940, 937, 946]),
    np.mean([1267, 1115, 1259, 1208, 1094])
]

# AES-256 values (average in µs)
aes256_us = [
    np.mean([548, 412, 410, 382, 382]),
    np.mean([438, 373, 342, 341, 401]),
    np.mean([625, 515, 465, 462, 502]),
    np.mean([481, 421, 444, 484, 428]),
    np.mean([972, 742, 792, 788, 807]),
    np.mean([742, 668, 690, 750, 736]),
    np.mean([1347, 1212, 1194, 1069, 1123]),
    np.mean([1495, 1375, 1509, 1320, 1318])
]

# Convert µs -> ms
aes128 = [v / 1000 for v in aes128_us]
aes256 = [v / 1000 for v in aes256_us]

# X positions
x = np.arange(len(groups))
bar_width = 0.35
gap = 0.05  # gap between bars in a pair

fig, ax = plt.subplots(figsize=(12,6))

# Plot bars: AES-128 left, AES-256 right
ax.bar(x - bar_width/2, aes128, bar_width, label='AES-128', color='orange')
ax.bar(x + bar_width/2, aes256, bar_width, label='AES-256', color='skyblue')

# Labels and title
ax.set_xticks(x)
ax.set_xticklabels(groups, rotation=45, ha='right')
ax.set_ylabel('Time (ms)')
ax.set_title('AES-128 vs AES-256 Performance by File Size and Operation (n=5)')
ax.legend()

# Add value labels above bars
for i in range(len(x)):
    ax.annotate(f'{aes128[i]:.2f}', xy=(x[i] - bar_width/2, aes128[i]),
                xytext=(0,3), textcoords='offset points', ha='center', va='bottom', fontsize=9)
    ax.annotate(f'{aes256[i]:.2f}', xy=(x[i] + bar_width/2, aes256[i]),
                xytext=(0,3), textcoords='offset points', ha='center', va='bottom', fontsize=9)

plt.tight_layout()
plt.show()
