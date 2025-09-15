# Pico Hybrid Cryptography: Classical vs Post-Quantum
Performance evaluation and benchmarking of classical and post-quantum cryptographic protocols on the Raspberry Pi Pico 2W microcontroller.

## Overview
This project implements and benchmarks two cryptographic protocols on resource-constrained hardware:

### Classical Protocol (Quantum-Vulnerable)
- RSA key establishment and signatures  
- AES-128-GCM encryption

### Post-Quantum Protocol (Quantum-Safe)
- ML-KEM key establishment  
- ML-DSA signatures  
- AES-256-GCM encryption

## Key Results
Our benchmarks on the Pico 2W show that post-quantum algorithms are competitive with classical ones:

- **Key Generation:** ML-KEM/ML-DSA are orders of magnitude faster than RSA  
- **Encryption:** AES-256 adds only ~0.2ms overhead vs AES-128  
- **Signatures:** ML-DSA signing significantly outperforms RSA  
- **Overall:** Quantum-safe protocols are viable for embedded systems

## Project Structure
```
pico-hybrid-crypto/
├── build/ # To be built
├── config/
│ └── user_settings.h # WolfSSL configuration
├── src/
│ ├── aes_benchmark.c # AES benchmarks
│ ├── mlkem_benchmark.c # ML-KEM benchmarks
│ ├── mldsa_benchmark.c # ML-DSA benchmarks
│ ├── rsaKeydis_benchmark.c # RSA key establishment
│ ├── rsaSign_benchmark.c # RSA signatures
│ └── bench_main.c # Main benchmark runner
├── CMakeLists.txt
└── memmap_custom.ld # Custom memory layout
```

## Prerequisites
**Hardware:** Raspberry Pi Pico 2W or compatible RP2350 board  
**Software:**
The ARM compiler and CMake need to be installed. In Debian / Ubuntu:

```
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```
## Quick Start

### Setup dependencies:
```
git clone https://github.com/raspberrypi/pico-sdk.git
git clone https://github.com/wolfSSL/wolfssl.git
export PICO_SDK_PATH=/path/to/pico-sdk
export WOLFSSL_ROOT=/path/to/wolfssl
```
### Build:
```
mkdir build && cd build
cmake -DPICO_BOARD=pico2_w ..
make
```

### Upload:
Hold BOOTSEL button, connect USB, drag benchmark.uf2 to the Pico
### View results:

In Linux, one can use the GNU Screen:
```
screen /dev/ttyACM0
```
To exit, press CTRL-A followed by CTRL-D
