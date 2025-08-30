from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP
import serial

ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)

# Generate RSA key pair
key = RSA.generate(2048)
pub_key = key.publickey().export_key()

# Send public key length + key
ser.write(len(pub_key).to_bytes(2, "big"))
print(len(pub_key))
print(pub_key)
ser.write(pub_key)

# Receive AES key length + encrypted AES key
# read length first (2 bytes)
enc_len_bytes = ser.read(2)
enc_len = int.from_bytes(enc_len_bytes, "big")

# now read exact ciphertext length
enc_aes = ser.read(enc_len)

# Decrypt AES key
cipher_rsa = PKCS1_OAEP.new(key)
aes_key = cipher_rsa.decrypt(enc_aes)
print("Received AES key:", aes_key.hex())
