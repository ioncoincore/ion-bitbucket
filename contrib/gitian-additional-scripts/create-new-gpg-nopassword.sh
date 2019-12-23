#!/bin/bash
KEYTYPE="RSA"
KEYLENGTH="4096"
SUBKEYTYPE="RSA"
SUBKEYLENGTH="4096"
YOURNAME="Your Name"
COMMENT="Some comment"
EMAIL="your@email.com"
EXPIREDATE="0"
EXPORTFOLDER=".gnupg"

USERNAME="yourusername" # required only for builds as root

#export GNUPGHOME="$(mktemp -d)"    # create a temp folder and do not import into current keyring
export GNUPGHOME="/home/$USERNAME/.gnupg"
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
     Creation-Date: $(date +%F)
     %no-ask-passphrase
     %no-protection
     # Do a commit here, so that we can later print "done" :-)
     %commit
     %echo done
EOF
gpg --batch --generate-key gpgtemplate
