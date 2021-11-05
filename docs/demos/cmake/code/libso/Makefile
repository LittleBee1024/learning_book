all: release

release: clean
	@cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=DEBUG
	cmake --build build --verbose

debug: clean
	@cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=DEBUG
	cmake --build build --verbose

clean:
	@rm -rf build

.PHONY: release debug clean
