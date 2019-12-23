#!/bin/bash
STARTTIMEID=$(date +%s)
ENFORCEDSTOP="180" # in minutes, default is 180 minutes, 3 hours
# gcloud vars
NAME="gitiantmp"
IMAGE="gitian-debian10" # ubuntu-minimal-1804-bionic-v20191217
DISKSIZE="60"   # In GB
IMAGEFAMILY="debian-10"
IMAGEPROJECT="debian-cloud"
ZONE="europe-west1-b"
CUSTOMCPU="6"
CUSTOMMEMORY=$(($CUSTOMCPU*1000+500))
CUSTOMVMTYPE="n2" # n1: Skylake or older (default), n2: VM's with CPU platform Cascade Lake(2-80vCPUs, 1-640GBs)
#MACHINETYPE="n1-highcpu-$CPUS" # please ensure that machine is available
DISKTYPE="ssd" # SCSI SSD
METADATAFILESTARTUP="metadata.startup.meta" # filepath
METADATASSHKEYSFILE="metadata.sshkeys.meta" # filepath

# ssh - will be added to console as default key
MYSSHKEY="id_ecdsa_ionian_official_releases_ecdsa_521b"

# GPG
KEYTYPE="RSA"
KEYLENGTH="4096"
SUBKEYTYPE="RSA"
SUBKEYLENGTH="4096"
YOURNAME="Gitian User"
COMMENT="Automatic build process ID $STARTTIMEID"
EMAIL="gitianuser@i2pmail.org"
EXPIREDATE="0"
USERNAME="gitianuser"

# gitian
SIGNER="RANDOMAUTOCREATION"
PREVIOUSVER="4.0.00"
VERSION="5.0.99"
# optional settings
SERVER="defaultuploadserver"
HASH="256"                    # hash new files
UPLOADFOLDER="ion-binaries"      # folder on UPLOAD server (will be created if not existing)
JOBS="$CUSTOMCPU"                         # number of jobs, default: 2
MEMORY=$(($CUSTOMMEMORY-500))                    # RAM to be used, default: 2000
OS="m"				            # default: lwm
GITURL="git@bitbucket.org/ioncoin/ion.git"
GITIANOPTS="--os $OS --jobs $JOBS --memory $MEMORY --detach-sign --no-commit --build --no-upload --hash $HASH --previousver $PREVIOUSVER"

# gitian SSH/UPLOAD
# ssh upload server (required for .ssh/config creation)
MYSSHUPLOADKEY="$MYSSHKEY"
SSHUPLOADKEYPRIVATE=$(cat < $HOME/.ssh/$MYSSHUPLOADKEY)
SSHUPLOADKEYPUBLIC=$(cat < $HOME/.ssh/$MYSSHUPLOADKEY.pub)
UPLOADHOST="123.123.123.123"
UPLOADHOSTPORT="2222"
UPLOADHOSTUSER="someuser"
UPLOADHOSTIDENTITY="~/.ssh/$MYSSHUPLOADKEY"
UPLOADHOSTRSA=$(ssh-keyscan -t rsa $UPLOADHOST)

# metadata
## metadata - ssh keys
## get upload private and public keys (ensure please that those are without password before using them)
SSHUPLOADKEYPRIVATE=$(cat < $HOME/.ssh/$MYSSHUPLOADKEY)
SSHUPLOADKEYPUBLIC=$(cat < $HOME/.ssh/$MYSSHUPLOADKEY.pub)
## metadata -ssh uploadserver keys 
## add additional public keys (if some other ssh key requires access, example build monitoring)
### add new keys in format:  USER:YOURPUBLICKEY      with user you define to which user this key should be assigned
SSHKEYS=$(SSHKEYS=$(cat < $HOME/.ssh/$MYSSHKEY.pub) && echo "gitianuser:$SSHKEYS")
cat <<EOF | tee $METADATASSHKEYSFILE
gitianuser:ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDa2RhUaA6nnl25WsY7tF7XZsx8fdUbtOejnL/htTuKEY6EoqDat222sLVyKpaoxssC3P5oPSTegQKlY9mRng4M4xBMIUsTMuwCVyuqglRbIoRTTibFTCMHjGGZUD6vX9iR35+6TF/ZycIzMSoah3WvJ5uIvo1yEVwOsipeJ5+xHAqduku+VQU5A7n4iyYacvu83aVj2VaHuxOZ3tCnmNBqNM058+PR4fK7oP0OPIMzwQVjXtU2dc7gT//oRxUYyGWGCDH2vD926p9BSczyDz2tUOPCSE0IMTedffSIbOZ89NAS10HrAfNOgy+KMtVBigUiFHe4QVpDHoBz8wM860hL fonebonea@cs-6000-devshell-vm-d7157bac-93fd-4e31-b9d5-15f2ce4ecb47
gitianuser:ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDs2SGZCHmw/7g3tHnfYaGcgkdN6oGdJyxYE5mVVnmEwaEcVNSqhKLa5Mg+2tak0eumKIv1u0+SgXMUpUE5Ft0En9LBOg1sOUGA5gz5riHVFoI7VQBKFKq2mrUWzKmQFoOKn23Ss4lK6ojVeOAtK/YUyWaTaEfhRNVuOoEmc4DK25tAlfSDpwUG8OK18uBPeSg/VVOffz06ISGhVUnOhTNDrx26bhSUlHQz7aYx93t5h/qf5TdkxkS8YGtCfMBrP2Qy3wwimidkzRs508XGVSA87dGww3Q4ofScy6slSVcZ+bGojn9oBbODpaIyNq/O/aFu2fc/XRk4uGC9aUpGpWel ckti@gitian-clean
$SSHKEYS
EOF

## metadata - startup script
cat <<EOF | tee $METADATAFILESTARTUP
# update system
sudo apt-get update && sudo apt-get upgrade -y

# go to gitianuser's home
cd $(sudo -i -u $USERNAME echo \$HOME)
sudo -i -u $USERNAME git clone --recurse-submodules http://bitbucket.org/ioncoin/ion.git
sudo -i -u $USERNAME cp ion/contrib/gitian-build.py gitian-build.py ./
sudo -i -u $USERNAME cp ion/contrib/gitian-additional-scripts/build-and-upload.sh ./
sudo -i -u $USERNAME cp ion/contrib/gitian-additional-scripts/create-new-gpg-nopassword.sh ./

sudo -i -u $USERNAME sed -i "s/RSA/$KEYTYPE/" ./create-new-gpg-nopassword.sh
sudo -i -u $USERNAME sed -i "s/4096/$KEYLENGTH/" ./create-new-gpg-nopassword.sh
sudo -i -u $USERNAME sed -i "s/RSA/$SUBKEYTYPE/" ./create-new-gpg-nopassword.sh
sudo -i -u $USERNAME sed -i "s/4096/$SUBKEYLENGTH/" ./create-new-gpg-nopassword.sh
sudo -i -u $USERNAME sed -i "s/Your Name/$YOURNAME/" ./create-new-gpg-nopassword.sh
sudo -i -u $USERNAME sed -i "s/Some comment/$COMMENT/" ./create-new-gpg-nopassword.sh
sudo -i -u $USERNAME sed -i "s/your@email.com/$EMAIL/" ./create-new-gpg-nopassword.sh
sudo -i -u $USERNAME sed -i "s/yourusername/$USERNAME/" ./create-new-gpg-nopassword.sh

sudo -i -u $USERNAME ./create-new-gpg-nopassword.sh

# get long and short key-id's
SIGNERLONG=$(sudo -i -u $USERNAME gpg --list-secret-keys --keyid-format LONG | awk 'NR>1{print $2}' | awk -F '[/]' '{print $2}')
SIGNERSHORT=$(sudo -i -u $USERNAME gpg --list-secret-keys --keyid-format SHORT | awk 'NR>1{print $2}' | awk -F '[/]' '{print $2}')

# add ssh upload keys for gitianuser
sudo -i -u $USERNAME echo "$SSHUPLOADKEYPRIVATE" > /home/$USERNAME/.ssh/$MYSSHUPLOADKEY
sudo -i -u $USERNAME echo "$SSHUPLOADKEYPUBLIC" > /home/$USERNAME/.ssh/$MYSSHUPLOADKEY.pub

## allow gitianuser to connect to upload server
echo "$UPLOADHOSTRSA" | sudo -i -u $USERNAME tee -a /home/$USERNAME/.ssh/known_hosts

## ensure that .ssh folder exists
sudo -i -u $USERNAME mkdir -p /home/$USERNAME/.ssh
### create ssh config for gitianuser
sudo -i -u $USERNAME echo "host defaultuploadserver
    Hostname $UPLOADHOST
    port $UPLOADHOSTPORT
    PreferredAuthentications publickey
    IdentityFile $UPLOADHOSTIDENTITY
    User $UPLOADHOSTUSER" > /home/$USERNAME/.ssh/config

## allow current user to connecto to upload server
mkdir -p ~/.ssh
## add ssh upload keys for gitianuser
echo "$SSHUPLOADKEYPRIVATE" > ~/.ssh/$MYSSHUPLOADKEY
echo "$SSHUPLOADKEYPUBLIC" > ~/.ssh/$MYSSHUPLOADKEY.pub
echo "$UPLOADHOSTRSA" | tee -a ~/.ssh/known_hosts

# set gitian build vars
export SIGNER="$SIGNERLONG"            # signer key-id, we use long ID in this script, short ID for filenames
export VERSION="$VERSION"              # ion core version
export PREVIOUSVER="$PREVIOUSVER"      # required for release notes, this can be a commit or a tag
# optional settings
export SERVER="$SERVER"                # server ip/hostname
export HASH="$HASH"                    # hash new files
export UPLOADFOLDER="$UPLOADFOLDER"    # folder on UPLOAD server (will be created if not existing)
export JOBS="$JOBS"                    # number of jobs, default: 2
export MEMORY="$MEMORY"                # RAM to be used, default: 2000
export OS="$OS"				           # default: lwm

sudo -i -u $USERNAME gpg --export-secret-key -a $SIGNERLONG > $SIGNERSHORT-$EMAIL.private.asc
sudo -i -u $USERNAME gpg --export -a $SIGNERLONG > $SIGNERSHORT-$EMAIL.public.asc
sudo -i -u $USERNAME gpg --export-secret-key $SIGNERLONG > $SIGNERSHORT-$EMAIL.private.gpg
sudo -i -u $USERNAME gpg --export $SIGNERLONG > $SIGNERSHORT-$EMAIL.public.gpg

sudo -i -u $USERNAME gpg --export -a $SIGNERLONG > $SIGNERSHORT-$EMAIL.keybundle.asc
sudo -i -u $USERNAME gpg --export-secret-key -a $SIGNERLONG >> $SIGNERSHORT-$EMAIL.keybundle.asc
sudo -i -u $USERNAME gpg --export $SIGNERLONG > $SIGNERSHORT-$EMAIL.keybundle.gpg
sudo -i -u $USERNAME gpg --export-secret-key $SIGNERLONG >> $SIGNERSHORT-$EMAIL.keybundle.gpg

# build ion-core
sudo -i -u $USERNAME ./gitian-build.py "$GITIANOPTS" "$SIGNER" "$VERSION" | sudo -i -u $USERNAME tee ./gitian-build-${STARTTIMEID}.log

# shutdown
shutdown -h now
EOF

consolestopandremove() {
    ssh-keygen -f "$HOME/.ssh/known_hosts" -R "$IPADDRESS"
    ssh gitianuser@$IPADDRESS

    gcloud compute instances stop $NAME --zone $ZONE
    echo DELETING
    echo "Y" | gcloud compute instances delete $NAME --zone $ZONE
    # create image with external ip accessable over ssh
    #gcloud compute instances create $NAME --zone $ZONE --custom-extensions --custom-cpu=$CUSTOMCPU --custom-memory=$CUSTOMMEMORY --custom-vm-type=$CUSTOMVMTYPE --image-family $IMAGEFAMILY --image-project $IMAGEPROJECT --boot-disk-device-name=$NAME --boot-disk-size=${DISKSIZE}GB --metadata-from-file=startup-script=$METADATAFILESTARTUP,ssh-keys=$METADATASSHKEYSFILE --preemptible
}

# create from prepared image
consolecreateandstart () {
    gcloud compute instances create $NAME --zone $ZONE \
        --custom-extensions --custom-cpu=$CUSTOMCPU --custom-memory=$CUSTOMMEMORY --custom-vm-type=$CUSTOMVMTYPE \
        --image $IMAGE \
        --boot-disk-device-name=$NAME --boot-disk-size=${DISKSIZE}GB \
        --metadata-from-file=startup-script=$METADATAFILESTARTUP,ssh-keys=$METADATASSHKEYSFILE \
        --preemptible

    echo "# how to shutdown enforeced (copy and paste):
    gcloud compute instances stop $NAME --zone $ZONE
    echo DELETING
    echo 'Y' | gcloud compute instances delete $NAME --zone $ZONE"
}


## CREATE AND START CONSOLE ##
STARTVMID=$(date +%s)
consolecreateandstart

# if preemptable
IPADDRESS=$(gcloud compute instances list | grep $NAME | awk '{print $6}')
# if not preemptable
#IPADDRESS=$(gcloud compute instances list | grep $NAME | awk '{print $5}')

ssh-keygen -f $HOME/.ssh/known_hosts -R $IPADDRESS
#ssh gitianuser@$IPADDRESS

# if preemptable
VMSTATUS=$(gcloud compute instances list | grep $NAME | awk '{print $7}')
# if not preemptable
#VMSTATUS=$(gcloud compute instances list | grep $NAME | awk '{print $6}')

REFRESHRATE="60"
while [ $VMSTATUS = "RUNNING" ]
do
  VMSTATUS=$(gcloud compute instances list | grep $NAME | awk '{print $7}')
  # if not preemptable
  #VMSTATUS=$(gcloud compute instances list | grep $NAME | awk '{print $6}')
  if [ $VMSTATUS = "TERMINATED" ]; then
    echo "VM $NAME finished, terminating and deleting..."
    consolestopandremove
    break
  fi
  # check if current time is greater than max allowed, stop and delete VM in that case
  CURRENTTIME=$(date +%s)
  ENFORCEDSTOPTIME=$(($STARTVMID + $(($ENFORCEDSTOP*60))))
  if [[ $CURRENTTIME -gt $ENFORCEDSTOPTIME ]]; then
    echo "VM $NAME reaching max runtime, terminating and deleting..."
    consolestopandremove
    break
  fi
  sleep $REFRESHRATE
done

BUILDTIMESINCESCRIPTSTART=$(( $(date +%s) - $STARTTIMEID ))
BUILDTIMESINCEVMCREATION=""
echo "##### RUNTIME SUMMARY #####"
echo "Script started: $STARTTIMEID"
echo "Script runtime: $BUILDTIMESINCESCRIPTSTART)"