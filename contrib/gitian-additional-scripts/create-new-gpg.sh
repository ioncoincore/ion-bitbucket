#!/bin/bash
KEYTYPE="RSA"
KEYLENGTH="4096"
SUBKEYTYPE="RSA"
SUBKEYLENGTH="4096"
YOURNAME="Your Name"
COMMENT="Some comment"
EMAIL="your@email.com"
EXPIREDATE="0"
PASSWORD="YourPassword"
EXPORTFOLDER="/home/gpgkeys"

export GNUPGHOME="$(mktemp -d)"
cat <<EOF | tee gpgtemplate
     %echo Generating a basic OpenPGP key
     Key-Type: $KEYTYPE
     Key-Length: $KEYLENGTH
     Subkey-Type: RSA
     Subkey-Length: $SUBKEYLENGTH
     Name-Real: $YOURNAME
     Name-Comment: $COMMENT
     Name-Email: $EMAIL
     Expire-Date: $EXPIREDATE
     Passphrase: $PASSWORD
     # Do a commit here, so that we can later print "done" :-)
     %commit
     %echo done
EOF
gpg --batch --generate-key gpgtemplate

mkdir -p $EXPORTFOLDER
cp -fR $GNUPGHOME $EXPORTFOLDER/
