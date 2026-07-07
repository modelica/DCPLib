################################################
# DCPLib build
#
# Pulls the repository and builds the library.
# Afterwards it builds available examples.
# Later, built examples will be run for testing
################################################

WORKDIR /src

# Clone the GitLab repository into the project directory
RUN git clone --recursive https://github.com/$GH_USER/$GH_PROJECT.git \
    && mv $GH_PROJECT /$GH_PROJECT

# Build using CMake and make; Installing the built library
RUN mkdir -p /$GH_PROJECT/build \
    && cd /$GH_PROJECT/build \
    && cmake .. \
    && cmake --build . --target install


# Build examples
COPY build_examples.sh /DCPLib/build_examples.sh
RUN chmod +x /DCPLib/build_examples.sh

WORKDIR /DCPLib
RUN ./build_examples.sh

