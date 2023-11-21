#!/bin/bash

# Prepare the directory
mkdir /root/ca/intermediate

cd /root/ca/intermediate
mkdir certs crl csr newcerts private
chmod 700 private
touch index.txt
echo 1000 > serial

# keeps track of certificate revocation lists
echo 1000 > /root/ca/intermediate/crlnumber

echo "# -------------------"
echo " Copying intermediate_ca.conf "
echo "# -------------------"

# get correct configration file for intermediate CA
cp ../../intermediate_ca.conf ./openssl.cnf

echo "# -------------------"
echo " Create intermediate Key"
echo "# -------------------"

cd /root/ca
# Create the intermediate key (password)
openssl genrsa -aes256 -out intermediate/private/intermediate.key.pem 4096

chmod 400 intermediate/private/intermediate.key.pem

echo "# -------------------"
echo " Create CSR "
echo "# -------------------"

# Create the CSR intermediate cert sign request (password) (info)
cd /root/ca
openssl req -config intermediate/openssl.cnf -new -sha256 \
      -key intermediate/private/intermediate.key.pem \
      -out intermediate/csr/intermediate.csr.pem

echo "# -------------------"
echo " Sign intermediate cert"
echo "# -------------------"

# Sign the intermediate CSR... (password)
cd /root/ca

cp ../root_ca.conf ./openssl.cnf

openssl ca -config openssl.cnf -extensions v3_intermediate_ca \
      -days 3650 -notext -md sha256 \
      -in intermediate/csr/intermediate.csr.pem \
      -out intermediate/certs/intermediate.cert.pem

chmod 444 intermediate/certs/intermediate.cert.pem

echo "# -------------------"
echo " Verify intermediate cert"
echo "# -------------------"
#verify intermediate cert
openssl x509 -noout -text \
      -in intermediate/certs/intermediate.cert.pem

openssl verify -CAfile certs/ca.cert.pem \
      intermediate/certs/intermediate.cert.pem


echo "# -------------------"
echo " Create chain file"
echo "# -------------------"

# create chain file
cat intermediate/certs/intermediate.cert.pem \
      certs/ca.cert.pem > intermediate/certs/ca-chain.cert.pem

chmod 444 intermediate/certs/ca-chain.cert.pem
