PY=py

default: fmt test

fmt:
	clang-format -i *.cpp *.hpp

test: test_python test_cpp

test_cpp: rebuild
	ctest --test-dir build -V

test_day_%: rebuild day_%.cpp
	ctest --test-dir build -V -R Day_$*

rebuild: configure FORCE
	cmake --build build
FORCE:

configure: CMakeLists.txt
	cmake -B build -G Ninja

day_%.cpp:
	$(PY) day_template.py $*

PYTHON_SOLUTIONS=day_11.py
test_python: $(PYTHON_SOLUTIONS)
	$(PY) -m unittest $(PYTHON_SOLUTIONS)
