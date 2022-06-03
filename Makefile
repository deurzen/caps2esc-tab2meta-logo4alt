all: caps2esclogo4alt

caps2esclogo4alt:
	cmake -S . -B build
	make -C build

release:
	cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
	make -C build

install:
	make -C build install

test: caps2esclogo4alt
	ctest --verbose --test-dir build

.PHONY: clean
clean:
	@rm -rf ./tags
	@rm -rf ./build
	@rm -f ./include/protocols/*
