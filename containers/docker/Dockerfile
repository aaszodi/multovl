# == MULTOVL Dockerfile ==

# The base image
# Always use a concrete tag, not "latest"!
# Ubuntu 24.04, multi-platform image
FROM ubuntu:noble-20241118.1

# Install MULTOVL
# Note that only the serial executable is installed
COPY bin/multovl /usr/local/bin/multovl

# Run MULTOVL
ENTRYPOINT [ "/usr/local/bin/multovl" ]

# By default just print the help
CMD [ "--help" ]
