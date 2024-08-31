ARG base=debian:bookworm-20240812-slim


FROM $base AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
	build-essential ca-certificates git pkg-config libfmt-dev libgit2-dev libcurl4-openssl-dev nlohmann-json3-dev libtbb-dev \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY .clang-format .
COPY .clang-tidy .
COPY Makefile .
COPY src ./src/

RUN make RELEASE=1 install


FROM $base

RUN apt-get update && apt-get install -y --no-install-recommends \
	libfmt-dev libgit2-dev libcurl4-openssl-dev nlohmann-json3-dev libtbb-dev \
	&& rm -rf /var/lib/apt/lists/*

COPY --from=builder /usr/local/bin/poac /usr/local/bin/poac

CMD ["poac"]
