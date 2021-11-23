# == MULTOVL Dockerfile ==

# The base image
# Always use a concrete tag, not "latest"!
FROM ubuntu:focal-20210416

# Install MULTOVL
COPY bin/multovl /usr/local/bin/multovl

# Run MULTOVL
ENTRYPOINT [ "/usr/local/bin/multovl" ]

# By default just print the help
CMD [ "--help" ]
