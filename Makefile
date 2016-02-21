all: test pmi.o train.o
test: pmi.o train.o
	g++ --std=c++14 -DVERBOSE -O2 test.cpp pmi.o train.o -o test
train.o: pmi.o
	g++ --std=c++14 -DVERBOSE -O2 train.cpp -c -o train.o
pmi.o:
	g++ --std=c++14 -DVERBOSE -O2 pmi.cpp -c -o pmi.o
clean:
	rm pmi.o train.o test