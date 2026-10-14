# Routines based on modular arithmetic
# Nothing fancy but can be exported to other countries
# get the job done for simple encryption


def encrypt(key, msg):
    encrypted = []
    for i, c in enumerate(msg):
        key_c = ord(key[i % len(key)])
        msg_c = ord(c)
        encrypted.append(chr((msg_c + key_c) % 127))
    return ''.join(encrypted)


def decrypt(key, encrypted):
    msg = []
    for i, c in enumerate(encrypted):
        key_c = ord(key[i % len(key)])
        enc_c = ord(c)
        msg.append(chr((enc_c - key_c) % 127))
    return ''.join(msg)


if __name__ == '__main__':
    key = 'This_is_my_awsome_secret_key'
    msg = 'Hello world i think this should work but long message maybe not'

    encrypted = encrypt(key, msg)
    decrypted = decrypt(key, encrypted)

    print('Message:', repr(msg))
    print('Key:', repr(key))
    print('Encrypted:', repr(encrypted))
    print('Decrypted:', repr(decrypted))
