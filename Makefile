all: serve

.PHONY: help serve build clean

help:
	@echo "targets:"
	@echo "\tserve"
	@echo "\tbuild"

serve:
	mkdocs serve --dev-addr 127.0.0.1:8888

build:
	mkdocs build

clean:
	@rm -rf ./site
