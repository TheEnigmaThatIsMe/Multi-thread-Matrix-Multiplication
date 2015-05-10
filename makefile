all: test

check: test
	build/test

analyze: analysis
	build/analysis > timings.txt
	gnuplot < plot.gnu

test: matrix_multiply.o build
	gcc test.c build/matrix_multiply.o -std=c99 -lpthread -g -Wall -o build/test

analysis: matrix_multiply.o build
	g++ analysis.cpp build/matrix_multiply.o -std=c++11 -lpthread -g -Wall -o build/analysis

matrix_multiply.o: build
	gcc -c matrix_multiply.c -std=c99 -g -Wall -o build/matrix_multiply.o

build:
	mkdir -p build

clean:
	rm -rf build/*
