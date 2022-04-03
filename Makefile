CXX=g++ -std=c++17
flag = -fopenmp
all: bin/ParSeqAl

bin/ParSeqAl: bin/ParSeqAl.o
	$(CXX) -o bin/ParSeqAl bin/ParSeqAl.o $(flag)
bin/ParSeqAl.o: src/ParSeqAl.cpp
	$(CXX) -o bin/ParSeqAl.o -c src/ParSeqAl.cpp $(flag)

clean:
	rm -rf bin/ParSeqAl.o bin/ParSeqAl data/aligned_out.csv
