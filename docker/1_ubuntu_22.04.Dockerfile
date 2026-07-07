ARG GH_USER="modelica"
ARG GH_PROJECT="DCPLib"
ARG LINUX_DISTRIBUTION_IMAGE="ubuntu"
ARG LINUX_DISTRIBUTION_TAG="22.04"

FROM ${LINUX_DISTRIBUTION_IMAGE}:${LINUX_DISTRIBUTION_TAG}

# LABEL about the custom image
LABEL maintainer="alexander.nikolic@v2c2.at"
LABEL version="0.1"
LABEL description="This is a custom Docker Image for \
the DCP installation."

# Globally defined ARGs need to be consumed in this scoped by using ARG after the FROM
ARG GH_USER
ARG GH_PROJECT
ARG LINUX_DISTRIBUTION_IMAGE
ARG LINUX_DISTRIBUTION_TAG
ARG DEBIAN_FRONTEND=noninteractive

RUN echo $GH_USER $GH_PROJECT $LINUX_DISTRIBUTION_IMAGE $LINUX_DISTRIBUTION_TAG
RUN cat /etc/os-release

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    build-essential \
    uuid-dev \
    libkrb5-dev \
    libcurl4-openssl-dev \
    libasio-dev \
    libxerces-c-dev \
    libzip-dev && \
    rm -rf /var/lib/apt/lists/* && \
    apt clean
