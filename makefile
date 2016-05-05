
EXE = split_module_files.exe

all: dirtree
	$(CXX) -std=c++11 -o bin/$(EXE) split_module_files/split_module_files.cpp -lboost_filesystem -lboost_system

dirtree:
	mkdir -p bin
	mkdir -p obj

clean:
	rm -rf *.sdf bin/*.ilk bin/*.pdb obj/*.log obj/*.idb obj/*.pdb

cleanall:
	rm -rf *.sdf bin obj


