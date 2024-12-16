PY=py

default: fmt test

fmt:
	clang-format -i *.cpp *.hpp
	$(PY) -m black *.py

test: test_python test_cpp

test_cpp: rebuild
	ctest --test-dir build -V

test_day_%: rebuild day_%.cpp
	ctest --test-dir build -V -R Day_$*

test_%: rebuild %.cpp %.hpp
	build\$*_test.exe 

rebuild: configure FORCE
	cmake --build build

configure: CMakeLists.txt
	cmake -B build -G Ninja

day_%.cpp:
	$(PY) day_template.py $*

PYTHON_SOLUTIONS=day_11.py
test_python: $(PYTHON_SOLUTIONS)
	$(PY) -m unittest $(PYTHON_SOLUTIONS)

# Depending on this will always force a target rebuild
FORCE:
