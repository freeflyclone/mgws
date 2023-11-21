# Certificates & Tools

Firefox does not use Windows Certificate store by default.  Use <code>"about:config"</code> to set **security.enterprise_roots.enabled** to **true**

This is ONLY of concern for our own certs, that we only use locally.

## Introduction
Browsers consider it a security risk (it probably is) to access a non-encrypted Websocket server from https:, and refuse to do so.

To play nice with browsers, (else what's the point?), we must support encrypted Websocket (wss:) connections.

- Suggested best practice is to fully support wss: in the development process.
  - We need SSL server certificates that authenticate with "localhost" and local intranet hosts.
    - Self-signed certificates WILL work, but only with "localhost" (for most browsers)
    - For hosts inside our firewalls, we need a "signed by a **Trusted Root Certification Authority** certificate.
      - Becoming a **Trusted Root Certification Authority** is easy, so we do that.

To that end, the journey of becoming a **Trusted Root Certificate Authority** is documented somewhat here. 

This is an evolving solution, and this documentation is mostly notes to Evan so he doesn't forget.

## Background
- Useful references
  - [OpenSSL Certificate Authority - Jamie Nguyen](https://jamielinux.com/docs/openssl-certificate-authority/)
  - [How to Create Your Own SSL Certificate Authority for Local HTTPS Development](https://deliciousbrains.com/ssl-certificate-authority-for-local-https-development/)
  - [Digital Certificates](https://www3.rocketsoftware.com/rocketd3/support/documentation/Uniface/10/uniface/security/certificates/digitalCertificates.htm?tocpath=Protecting%20Your%20Application%7CTransport%20Protocols%20and%20Certificates%7CDigital%20Certificates%7C_____0)
  - [SSL Certificate Format](https://www.tutorialsteacher.com/https/ssl-certificate-format)
  - [OpenSSL Official Website](https://www.openssl.org/)

- Assumptions:
  - Reader understands basic SSL concepts, the difference between http: and https:, and ws: vs wss:

- Terms:
  - CA : Certification Authority
  - chain of trust : starts with a root CA, possibly multiple intermediate CA, used to sign server certificates
    
Client apps (browsers, Python scripts, boost::beast code, etc) validate a certificate by verifying it was signed by **Trusted Root Certification Authority**.  Most SSL-aware client apps have a list of valid root CA sources they consult.  

- Modern OS's have a system-wide store of root CA certificates
  - A defacto chain of trust, prompted by financial interests operating on the Web
  - These are populated with root CA certicates provided by commercial interests
    - Commercial Trust vendors
    - OS vendors
  - Browser vendors *may* use these root CA certificates
    - Individual server exceptions can be added to browsers, not particularly useful though.
    - Firefux specifically does NOT use system cert store by default, favoring their own baked-in CA list instead.
      - This was discovered during development, hence the comment at the top of this page.
      - It makes sense historically.  Netscape heavily influenced the origin of SSL, everyone else is still catching up.
    - Tested thus far:
      - Google Chrome
      - Microsoft Edge
      - iPad Safari  
  - Other client apps *might* use their own root CA lists
    - Python scripts
    - boost::beast code
  - Windows and Linux both have mechanisms to add to this system-wide store
    - We exploit this.

## Tools
The sites mentioned in "Useful references" above utlize the openssl app to generate various certificates.  The tools folder contains scripts derived from those references.

- Tools folder:
  - Config files - used to specify defaults for various certificates created.
    - root_ca.conf - configures the Root Certification Authority certificate
    - intermediate_ca.conf - configures the Intermediate Certifiation Authority certificate
    - server.conf - configures an individual server's SSL certificate
  - Scripts - derived from various of the "Useful references" sites. VERY unsophisticated thus far.
    - setup_root_ca.sh - sets up the Root Certification Authority certificate
      - Run this first
    - setup_intermediate_ca.sh - sets up Intermediate Certification Authority certificates
      - Run this after **setup_root_ca.sh**
      - Signed by the Root CA certificate
        - Creates a chain of trust, suitable for signing server certificates
      - Any number of Intermediate CA certificates can be issued
        - Tim can have his own Intermediate CA certificate, as can Evan
    - make_server_cert.sh - creates a server certificate signed by an Intermediate CA
      - Run this after **setup_intermediate_ca.sh**
      - Now supports "Subject Alternative Name" fields
        - Which means the same certificate can be used on a varity of hosts
          - Evan added some that are useful to him.
    - export_ca.sh - copy & rename Root CA and Intermiate CA certs from where they are generated to where they get used
      - Defaults favor Evan's environment
    - export_host.hs - copy & rename signed Server cert from where it is generated to where it will be used.
      - Defaults favor Evan's environment  
          
To effectively use these tools, it is useful to understand the context of their development.  
- Run on a Raspberry Pi
  - Running Ubuntu 22.04 Server
  - Logged in over SSH
  - run as root with <code>sudo -i</code>
    - That is: root priveleges, and running in root's default home folder of **/root**
    - The contents of the **tools** folder must be copied to /root, by root user, in order to function as intended.

## Deployment
### Windows
- Install **ca.cert.crt** in the "Trusted Root Certification Authorities" store using the **Certificate Import Wizard**
  - Right-click on the file in Windows Explorer and chose "Install Certificate", to launch the "Certificate Import Wizerd"
    - Choose "Current User".
      - Click "Next"
    - Choose "Place all certificates in the following store"
      - Click "Browse..."
        - Choose "Trusted Root Certification Authorities"
        - Click "Ok"
      - Click "Next"
    - Verify contents of "You have specified the following settings:" panel
      - Click "Finish"

- Install **ca-chain.cert.crt** in the "Intermediate Certification Authorities" store using the **Certificate Import Wizard**
  - Right-click on the file in Windows Explorer and chose "Install Certificate", to launch the "Certificate Import Wizerd"
    - Choose "Current User".
      - Click "Next"
    - Choose "Automatically select the certificate store basedon the type of certificate"
      - Click "Next"
    - Verify contents of "You have specified the following settings:" panel
      - Click "Finish"

- Verify proper installation
  - Press "WindowsKey" + R
    - Enter "certmgr.msc" in the box
      - Click "Ok"
  - In the left pane, expand "Trusted Root Certification Authorities" and click on "Certificates"
    - Observerve "Beastie Games" in the right side pain
  - In the left pain, expand "Intermediate Root Certification Authorities" and click on "Certificates"
    - Observerve "Beastie Games" in the right side pain

### Linux
- Make sure the ca-certificates pkg has been installed.
- Add /usr/share/ca-certificates/beastie folder, as root
- Copy **ca-chain.cert.crt** to /usr/share/ca-certificates/beastie (create if needed)
- Run <code>sudo dpkg-reconfigure ca-certificates</code> as root
- Verify with <code>awk -v cmd='openssl x509 -noout -subject' '/BEGIN/{close(cmd)};{print | cmd}' < /etc/ssl/certs/ca-certificates.crt | grep Beastie</code>

Results should be similar to:
<pre>
   subject=C = US, ST = Washington, O = Beastie Games LLC, OU = Development, CN = Beastie Games LLC, emailAddress = WeDontLoveYou@.apple.com
   subject=C = US, ST = Washington, L = Washougal, O = Beastie Games LLC, OU = Development, CN = Beastie Games LLC, emailAddress = WeDontLoveYou@.apple.com
</pre>
## Various Notes
### Firefox Restart
- Scenario - Windows host, no Beastie certificates installed, Firefox running...
  - Install the certificates as described in the Deployment/Windows section above
  - Reloading the URL still shows security warning
  - Restart Firefox, and the problem should be solved.

## Future Uses
It may be worthwhile to invest in proper certificate infrastructure, built by us. Monetization via selling subscription certificates, for example.
