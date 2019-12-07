#!/bin/bash

# setup
# ./gitian-build.py --setup --build $SIGNER $VERSION SHA256 ion-biaries
# see gitian build.py for further options

# build from branch/commit
# defaultuploadserver: if you pass it, you have to pass username/password, best was is to configure it in your .ssh/config, example config:
# 	host defaultuploadserver
# 	  HostName 192.168.1.234
# 	  PreferredAuthentications publickey
# 	  IdentityFile ~/.ssh/id_ecdsa
# uploadlogs: if used, all logs will be uploaded too
# uploadfolder: folder on a server where binaries should be placed
# hash: create hashes for new files
#./gitian-build.py --detach-sign --commit --no-commit --build --upload defaultuploadserver --uploadlogs --uploadfolder ion-binaries --hash SHA256 $SIGNER $VERSION
# build from tag/version
#./gitian-build.py --detach-sign --no-commit --build --upload defaultuploadserver --uploadlogs --uploadfolder ion-binaries --hash SHA256 $SIGNER $VERSION

cd $HOME

# Signer ID,Name, Email ... (use ID listed in gpg --list-keys)
export SIGNER="7FDF93BCDE169B04"
# version (tag)/branch/hash
export VERSION="develop"

CURRENTDIR=${PWD}

# To setup run:
# ./gitian-build.py --setup ${SIGNER} ${VERSION}
# sudo reboot

# cleanup
rm -fR ./ion ./gitian.sigs ./ion-detached-sigs ./ion-binaries

# download and update
# over ssh (use SSH with key added to github without password which lets script auto push sigs and whatever you have preset)
# git clone git@github.com:ioncoin/ion
# over https
if [ ! -d ./ion ]; then
    git clone git@bitbucket.org:ioncoin/ion.git
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
    cd ${CURRENTDIR}
fi

./gitian-build.py --detach-sign --commit --no-commit --build --upload defaultuploadserver --uploadlogs --uploadfolder ion-binaries --hash SHA256 $SIGNER $VERSION
