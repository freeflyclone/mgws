#!/bin/bash

DEST=~evan/src/mgws/certs
OWNER=evan

CERT_PEM_EXTENSION=".cert.pem"
CRT_EXTENSION=".cert.crt"

ROOT_CA="ca"
INTERMEDIATE_CA="ca-chain"

if [ $# -gt 1 ]
then
    DEST=$1
fi

if [ $# -gt 2 ]
then
    OWNER=$2
fi

echo "Exporting root_ca, intermediate_ca to ${DEST}, owner will be ${OWNER}:${OWNER}"
read -p "[enter] to continue, else ^C to abort..."

cp ca/certs/${ROOT_CA}${CERT_PEM_EXTENSION}  ${DEST}/${ROOT_CA}${CRT_EXTENSION}
cp ca/intermediate/certs/${INTERMEDIATE_CA}${CERT_PEM_EXTENSION}  ${DEST}/${INTERMEDIATE_CA}${CRT_EXTENSION}

chown ${OWNER}:${OWNER} ${DEST}/${ROOT_CA}${CRT_EXTENSION}
chown ${OWNER}:${OWNER} ${DEST}/${INTERMEDIATE_CA}${CRT_EXTENSION}
