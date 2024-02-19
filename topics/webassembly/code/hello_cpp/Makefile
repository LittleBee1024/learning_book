all: build

TARGET = api
# malloc and free is from clib which is used by allocateUTF8 in js,
# they are not exported by default
CFLAG = --no-entry -s "EXPORTED_FUNCTIONS=['_malloc', '_free']"

build: clean
	em++ -std=c++20 $(TARGET).cpp $(CFLAG) -o $(TARGET).js

dump:
	wasm2wat $(TARGET).wasm -o $(TARGET).wat

clean:
	@rm -rf *.js *.wasm *.wat

serve:
	python -m SimpleHTTPServer

.PHONY: all build dump clean serve
