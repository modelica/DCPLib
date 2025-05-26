#!/usr/bin/env bash
OS="redhat" # Pass any of ubuntu, redhat, rockylinux
VER="ubi8"  # ubuntu: 18.04, 22.04; redhat: ubi8, ubi9; rockylinux: 8, 9

DOCKER=docker # You might use podman as well
DOCKERFILE=Dockerfile.tmp


cat docker/1_${OS}_$VER.Dockerfile > $DOCKERFILE
cat docker/2_build.Dockerfile >> $DOCKERFILE
cat docker/3_run_test.Dockerfile >> $DOCKERFILE

# During development you might remove the --no-cache option for faster rebuilds.
$DOCKER build -t dcp/${OS}_$VER -f $DOCKERFILE \
  --build-arg LINUX_DISTRIBUTION_IMAGE=$OS \
  --build-arg LINUX_DISTRIBUTION_TAG=$VER . \
  --no-cache

# If you want to run the last built image comment out the following two lines
#$DOCKER run --name testcontainer -it localhost/dcp/${OS}_$VER:latest
#$DOCKER container rm testcontainer
