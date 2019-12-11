FROM debian:stretch
LABEL maintainer="Ion Developers <dev@ionomy.com>"
LABEL description="Dockerised ioncoin, built from Travis"

RUN apt-get update && apt-get -y upgrade && apt-get clean && rm -fr /var/cache/apt/*

COPY bin/* /usr/bin/
