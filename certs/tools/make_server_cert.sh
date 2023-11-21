#!/bin/bash

URL=${URL:-hq.local}

if [ $# -ne 0 ]
then
    URL=$1
fi

echo "using: \'${URL}\' for key & cert names"


cd /root/ca

echo "# -------------------"
echo " Create server key "
echo "# -------------------"

openssl genrsa \
    -out intermediate/private/${URL}.key.pem  \
    2048

chmod 400 intermediate/private/${URL}.key.pem


echo "# -------------------"
echo " Create CSR "
echo "# -------------------"

cd /root/ca
openssl req -config ../server.conf \
    -key intermediate/private/${URL}.key.pem \
    -new \
    -sha256 \
    -out intermediate/csr/${URL}.csr.pem \
    

#echo "premature end for progress check." && exit 0;

echo "# -------------------"
echo " Sign server cert"
echo "# -------------------"

cd /root/ca

openssl ca -config intermediate/openssl.cnf \
    -extensions server_cert \
    -days 365 \
    -notext -md sha256 \
    -in intermediate/csr/${URL}.csr.pem \
    -out intermediate/certs/${URL}.cert.pem \
    -extensions req_ext \
    -extfile ../server.conf

#openssl ca -config intermediate/openssl.cnf \
      #-extensions server_cert -days 375 -notext -md sha256 \
      #-in intermediate/csr/${URL}.csr.pem \
      #-out intermediate/certs/${URL}.cert.pem


chmod 444 intermediate/certs/${URL}.cert.pem

echo "# -------------------"
echo " Verify server cert"
echo "# -------------------"

openssl x509 -noout -text \
      -in intermediate/certs/${URL}.cert.pem   


echo "# -------------------"
echo " Verify chain of trust "
echo "# -------------------"

openssl verify -CAfile intermediate/certs/ca-chain.cert.pem \
      intermediate/certs/${URL}.cert.pem

