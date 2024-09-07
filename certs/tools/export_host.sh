#!/bin/bash

HOST=localhost
DEST=~evan/src/mgws/certs
OWNER=evan

CERT_PEM_EXTENSION=".cert.pem"
KEY_PEM_EXTENSION=".key.pem"
CRT_EXTENSION=".crt"
KEY_EXTENSION=".key"

if [ $# -ge 1 ]
then
	echo "Setting host to $1"
    HOST=$1
fi

if [ $# -ge 2 ]
then
    DEST=$2
fi

if [ $# -ge 3 ]
then
    OWNER=$3
fi

echo "Exporting ${HOST} to ${DEST}, owner will be ${OWNER}:${OWNER}"
read -p "[enter] to continue, else ^C to abort..."

cp ca/intermediate/certs/${HOST}${CERT_PEM_EXTENSION}  ${DEST}/${HOST}${CRT_EXTENSION}
cp ca/intermediate/private/${HOST}${KEY_PEM_EXTENSION} ${DEST}/${HOST}${KEY_EXTENSION}

chown ${OWNER}:${OWNER} ${DEST}/${HOST}${CRT_EXTENSION}
chown ${OWNER}:${OWNER} ${DEST}/${HOST}${KEY_EXTENSION}
chmod 444 ${DEST}/${HOST}.*
