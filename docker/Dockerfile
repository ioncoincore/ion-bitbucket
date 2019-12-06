FROM debian:stretch
LABEL maintainer="Ion Developers <dev@ion.org>"
LABEL description="Dockerised IonCore, built from Travis"

RUN apt-get update && apt-get -y upgrade && apt-get clean && rm -fr /var/cache/apt/*

COPY bin/* /usr/bin/
