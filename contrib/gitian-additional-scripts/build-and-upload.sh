#!/bin/bash
cd $HOME

# mandatory settings
export SIGNER="7FDF93BCDE169B04"        # Signer ID,Name, Email ... (use ID listed in gpg --list-keys)
export VERSION="develop"                # version (tag)/branch/hash

# optional settings
export UPLOAD="defaultuploadserver"
export HASH="SHA256"                    # hash new files
export UPLOADFOLDER="ion-binaries"      # folder on UPLOAD server (will be created if not existing)

# To setup run and reboot:
#   ./gitian-build.py --setup ${SIGNER} ${VERSION}
#   sudo reboot

# build from branch/commit
#   defaultuploadserver: if you pass it, you have to pass username/password, best was is to configure it in your .ssh/config, example config:
#     	host defaultuploadserver
#     	  HostName 192.168.1.234
#     	  PreferredAuthentications publickey
#     	  IdentityFile ~/.ssh/id_ecdsa
#   uploadlogs: if used, all logs will be uploaded too
#   uploadfolder: folder on a server where binaries should be placed
#   hash: create hashes for new files
# command examples
#   build from branch/commit (with --commit switch)
#       ./gitian-build.py --detach-sign --commit --no-commit --build --upload defaultuploadserver --uploadlogs --uploadfolder ion-binaries --hash SHA256 $SIGNER $VERSION
#   build from tag/version (without --commit switch)
#       ./gitian-build.py --detach-sign --no-commit --build --upload defaultuploadserver --uploadlogs --uploadfolder ion-binaries --hash SHA256 $SIGNER $VERSION

if [ ! -d ./ion ]; then
    git clone http://bitbucket.org/ioncoin/ion.git
else
    cd ion;
    git fetch origin -f;
    git pull -f;
    cd ..
fi

# copy gitian-build.py script to current dir (normally home)
if [ -f ./ion/contrib/gitian-build.py ]
    cp -f ./ion/contrib/gitian-build.py ./gitian-build.py
    chmod +x ./gitian-build.py
else
    echo "ERROR: Can not find gitian-build script"
    exit
fi

if [ ! -d ./ion-detached-sigs ]; then
    git clone https://bitbucket.org/ioncoin/ion-detached-sigs
fi
if [ ! -d ./gitian.sigs ]; then
    git clone https://bitbucket.org/ioncoin/gitian.sigs
fi

# update gitian-builder
if [ ! -d ./gitian-builder ]; then
    # download
    git clone https://github.com/devrandom/gitian-builder.git
else
    # update
    cd ./gitian-builder;
    git pull
    cd ..
fi

./gitian-build.py --detach-sign --commit --no-commit --build --upload defaultuploadserver --uploadlogs --uploadfolder ion-binaries --hash SHA256 $SIGNER $VERSION
