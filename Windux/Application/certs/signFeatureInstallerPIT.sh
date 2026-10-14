#!/bin/bash

FEATURE=${1}
FEATURESIG=${1}.signed

KEY=ProductIntegrationSigningCertKey.pem
CRT=ProductIntegrationSigningCert.crt

# -----------------------------------------------------------------------------

if [ -f $FEATURE ];
then
	echo "signing $FEATURE..."
else
	echo "target $FEATURE does not exist!"
	exit -1
fi

openssl dgst -sha256 -sign $KEY -passin pass:authenticpit -out $FEATURESIG $FEATURE

if [ $? -ne 0 ]; then
	echo "failed to sign feature!"
	exit -1
fi

cp $CRT $FEATURE.crt

if [ -f $FEATURESIG ];
then
	echo "done!"
else
	echo "error signing feature!"
	exit -1
fi

