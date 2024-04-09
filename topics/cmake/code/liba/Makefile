all: release

release: clean
	@cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=DEBUG
	cmake --build build --verbose

debug: clean
	@cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=DEBUG
	cmake --build build --verbose

clean:
	@rm -rf build

web: clean
	emcmake cmake -S . -B build
	cmake --build ./build --verbose

.PHONY: release debug clean web
