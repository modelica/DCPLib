ARG GH_USER="modelica"
ARG GH_PROJECT="DCPLib"
ARG LINUX_DISTRIBUTION_IMAGE="redhat"
ARG LINUX_DISTRIBUTION_TAG="ubi8"

FROM docker.io/${LINUX_DISTRIBUTION_IMAGE}/${LINUX_DISTRIBUTION_TAG}

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

RUN echo $GH_USER $GH_PROJECT $LINUX_DISTRIBUTION_IMAGE $LINUX_DISTRIBUTION_TAG
RUN cat /etc/redhat-release

# It might be necessary to enable EPEL, see https://www.redhat.com/en/blog/install-epel-linux
# dnf install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
# DOES HAVE xerces-c-devel
# DOES NOT HAVE asio-devel
RUN dnf -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm

RUN dnf -y update && \
    dnf -y install --nodocs \
    zip \
    git \
    cmake \
    gcc \
    gcc-c++ \
    libstdc++-static \
    make \
    kernel-headers \
    libzip-devel \
    openssl-devel \
    xerces-c-devel && dnf clean all -y && rm -rf /var/cache/dnf

WORKDIR /src

RUN curl -L -o "asio.zip" -O https://sourceforge.net/projects/asio/files/asio/1.12.2%20%28Stable%29/asio-1.12.2.zip/download
RUN unzip asio.zip
RUN cd asio-1.12.2/ && ./configure --without-boost && make && make install && cd ..
