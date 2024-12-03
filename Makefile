default: fmt test

fmt:
	clang-format -i *.cpp *.hpp

test: rebuild
	ctest --test-dir build -V

test_%: rebuild
	build/$*_test

rebuild: configure FORCE
	cmake --build build
FORCE:

configure: CMakeLists.txt
	cmake -B build -G Ninja
