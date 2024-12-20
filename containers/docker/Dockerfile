# == MULTOVL Dockerfile ==

# The build context directory is ${CMAKE_CURRENT_BINARY_DIR}
# and this Dockerfile is copied there by CMake

# The base image
# Always use a concrete tag, not "latest"!
# Ubuntu 24.04, multi-platform image
FROM ubuntu:noble-20241118.1

ARG MULTOVL_VERSION
LABEL org.opencontainers.image.authors="András Aszódi, aszodi@interquadrat.eu" \
    org.opencontainers.image.description="https://dx.doi.org/10.1093/bioinformatics/bts607" \
    org.opencontainers.image.title="multovl" \
    org.opencontainers.image.version="${MULTOVL_VERSION}"

# MULTOVL executables (all must be statically linked)
COPY config_info multovl multovlprob parmultovlprob /usr/local/bin
# ...auxiliary scripts
COPY scripts/* /usr/local/bin/
# ...and the entrypoint script
COPY docker_runner.sh /usr/local/bin

# Prints the help by default
ENTRYPOINT [ "/usr/local/bin/docker_runner.sh" ]
