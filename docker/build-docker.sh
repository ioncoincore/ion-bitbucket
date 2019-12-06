#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/..

DOCKER_IMAGE=${DOCKER_IMAGE:-ionpay/iond-develop}
DOCKER_TAG=${DOCKER_TAG:-latest}

BUILD_DIR=${BUILD_DIR:-.}

rm docker/bin/*
mkdir docker/bin
cp $BUILD_DIR/src/iond docker/bin/
cp $BUILD_DIR/src/ion-cli docker/bin/
cp $BUILD_DIR/src/ion-tx docker/bin/
strip docker/bin/iond
strip docker/bin/ion-cli
strip docker/bin/ion-tx

docker build --pull -t $DOCKER_IMAGE:$DOCKER_TAG -f docker/Dockerfile docker
