#!/bin/bash
cd $HOME

# mandatory settings
export SIGNER="E3FBDA7F43638C28"        # Signer ID,Name, Email ... (use ID listed in gpg --list-keys)
#export VERSION="master"                # version (tag)/branch/hash
export VERSION="5.0.99"       # version (tag)/branch/hash
export PREVIOUSVER="4.0.00"
# optional settings
export SERVER="defaultuploadserver"
export HASH="256"                    # hash new files
export UPLOADFOLDER="ion-binaries"      # folder on UPLOAD server (will be created if not existing)
export JOBS="6"                         # number of jobs, default: 2
export MEMORY="8000"                    # RAM to be used, default: 2000
export OS="lwm"				            # default: lwm

# get more info about gitian-build options/syntax with gitian-build.py --help
# usage: gitian-build.py [-h] [-c] [-p] [-u URL] [-v] [-b] [-s] [-B] [-o OS]
#                        [-j JOBS] [-m MEMORY] [-k] [-d] [-S] [-D] [-n] [-z]
#                        [-x SERVER] [-l] [-f UPLOADFOLDER] [-y HASH]
#                        [signer] [version]

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
rm -f ./gitian-build.py ./gitian-builder/var/build.log
rm -fR ./ion ./gitian-builder/inputs/ion* ./gitian-builder/cache/ion* ./gitian.sigs

# cleanup
if [ ! -d ./ion ]; then
    git clone --recurse-submodules http://bitbucket.org/ioncoin/ion.git
fi

# copy gitian-build.py script to current dir (normally home)
if [ -f ./ion/contrib/gitian-build.py ]; then
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

# from branch, with upload
#./gitian-build.py --os $OS --jobs $JOBS --memory $MEMORY --detach-sign --commit --no-commit --build --server $SERVER --uploadlogs --uploadfolder $UPLOADFOLDER --hash $HASH --previousver $PREVIOUSVER $SIGNER $VERSION

# from tag, with upload
#./gitian-build.py --os $OS --jobs $JOBS --memory $MEMORY --detach-sign --no-commit --build --server $SERVER --uploadlogs --uploadfolder $UPLOADFOLDER --hash $HASH --previousver $PREVIOUSVER $SIGNER $VERSION

# sign from tag, with upload
#./gitian-build.py --os $OS --jobs $JOBS --memory $MEMORY --detach-sign --no-commit --sign --server $SERVER --uploadlogs --uploadfolder $UPLOADFOLDER --hash $HASH --previousver $PREVIOUSVER $SIGNER $VERSION

# from tag, no upload
./gitian-build.py --os $OS --jobs $JOBS --memory $MEMORY --detach-sign --no-commit --build --no-upload --hash $HASH --previousver $PREVIOUSVER $SIGNER $VERSION
