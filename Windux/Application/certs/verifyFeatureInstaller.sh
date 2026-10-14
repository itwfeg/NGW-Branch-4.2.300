#!/bin/bash

FEATURE=${1}
FEATURESIG=${1}.signed

KEY=/tmp/publickey.pem
CHAIN=/tmp/cert-chain.crt
CRT=${1}.crt


## Extract key
#openssl pkey -in foo.pem -out foo-key.pem
#
## Extract chain
#openssl crl2pkcs7 -nocrl -certfile foo.pem |
#  openssl pkcs7 -print_certs -out foo-chain.pem
#
## Extract cert
#openssl x509 -in foo.pem -outform DER -out foo-cert.pem

# -----------------------------------------------------------------------------

if [ -f $FEATURE ];
then
	echo "verifying $FEATURE..."
else
	echo "target $FEATURE does not exist!"
	exit 1
fi

# Extract certificate chain
openssl crl2pkcs7 -nocrl -certfile $CRT | openssl pkcs7 -print_certs -out $CHAIN

# Verification of the certificate
openssl verify -untrusted $CHAIN $CRT &> /dev/null

if [ $? -ne 0 ]; then
	echo "invalid certificate!"
	exit 2
fi

openssl verify -untrusted $CHAIN $CRT | grep 'error' &> /dev/null

if [ $? -eq 0 ]; then

	openssl verify -untrusted $CHAIN $CRT | grep 'not yet valid' &> /dev/null

	if [ $? -eq 0 ]; then
		echo "certificate not yet valid!"
		exit 3
	fi

	openssl verify -untrusted $CHAIN $CRT | grep 'has expired' &> /dev/null

	if [ $? -eq 0 ]; then
		echo "certificate expired!"
		exit 4
	fi

	echo "certificate not accepted!"
	exit 5
fi

# Extracting the public key from the certificate:
openssl x509 -in $CRT -pubkey -noout > $KEY

# Verification of the signature
openssl dgst -sha256 -verify $KEY -signature $FEATURESIG $FEATURE &> /dev/null

if [ $? -ne 0 ]; then
	echo "failed verification!"
	exit 6
fi

echo "OK"

