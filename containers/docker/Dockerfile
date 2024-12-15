# == MULTOVL Dockerfile ==

# The base image
# Always use a concrete tag, not "latest"!
# Ubuntu 24.04, multi-platform image
FROM ubuntu:noble-20241118.1

# Current version
ENV MULTOVL_VERSION=1.5

LABEL org.opencontainers.image.authors="András Aszódi, aszodi@interquadrat.eu" \
    org.opencontainers.image.description="http://dx.doi.org/10.1093/bioinformatics/bts607" \
    org.opencontainers.image.title="multovl" \
    org.opencontainers.image.version="${MULTOVL_VERSION}"

# Install MULTOVL executables (all statically linked)
COPY bin/* /usr/local/bin/
# ...and the entrypoint script
COPY docker_runner.sh /usr/local/bin

# Prints the help by default
ENTRYPOINT [ "/usr/local/bin/docker_runner.sh" ]
