CXX=g++ -std=c++17
flag = -fopenmp
all: bin/Prefix

bin/Prefix: bin/Prefix.o
	$(CXX) -o bin/Prefix bin/Prefix.o $(flag)
bin/Prefix.o: src/Prefix.cpp
	$(CXX) -o bin/Prefix.o -c src/Prefix.cpp $(flag)

clean:
	rm -rf bin/Prefix.o bin/Prefix data/aligned_out.csv