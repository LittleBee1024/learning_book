all: serve

.PHONY: help serve build clean

help:
	@echo "targets:"
	@echo "\tserve"
	@echo "\tbuild"

serve:
	mkdocs serve

build:
	mkdocs build

clean:
	@rm -rf ./site
