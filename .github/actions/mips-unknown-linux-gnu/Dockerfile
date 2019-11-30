FROM docker.pkg.github.com/poacpm/dockerfiles/mips-unknown-linux-gnu:gcc-9
# Copies your code file from your action repository to the filesystem path `/` of the container
COPY ../entrypoint.sh /entrypoint.sh
# Code file to execute when the docker container starts up (`entrypoint.sh`)
ENTRYPOINT ["/entrypoint.sh"]
