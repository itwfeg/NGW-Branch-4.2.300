#!/bin/bash

# This script copys root cert and code signing cert from ca folder.
#
# For example, run the following:
# $ ./copyCerts.sh 3.2


# check for version passed in
if [ $# -eq 1 ]; then
    VERSION=$1
else
    echo "Please specify a version (i.e. '3.3')!"
    exit 1
fi

# setup key and crt
DIR1=codeSigningCA
KEY1=HobartCodeSigningCertKey.pem
CRT1=HobartCodeSigningCert.crt

DIR2=productIntegrationCA
KEY2=ProductIntegrationSigningCertKey.pem
CRT2=ProductIntegrationSigningCert.crt

ROOTCRT=HobartRootCA.crt

# -----------------------------------------------------------------------------

# generate code signing cert
cat ca/$DIR1/certs/hobartcert-$VERSION.pem ca/$DIR1/certs/intermediatecert.pem > $CRT1
cat ca/$DIR2/certs/pitcert-$VERSION.pem ca/$DIR2/certs/intermediatecert.pem > $CRT2

# copy code signing cert key
cp ca/$DIR1/private/hobartkey.pem $KEY1
cp ca/$DIR2/private/pitkey.pem $KEY2

# copy root ca
cp ca/certs/cacert.pem $ROOTCRT
