#TODO: write a proper makefile

CXX=gcc
BIN=vectplot
COMPFLAGS=-O3
LINKFLAGS=-lm -lX11

all: main

main:
	$(CXX) -c -o src/calc.o src/calc.c $(COMPFLAGS)
	$(CXX) -c -o src/plot.o src/plot.c $(COMPFLAGS)
	$(CXX) -c -o src/vectplot.o src/vectplot.c $(COMPFLAGS)
	$(CXX) -o $(BIN) src/calc.o src/plot.o src/vectplot.o $(COMPFLAGS) $(LINKFLAGS)
debug:
	$(CXX) -o $(BIN).debug src/calc.c src/plot.c src/vectplot.c $(LINKFLAGS) -O0 -g
clean:
	rm -f vectplot vectplot.debug src/*.o
run:
	rtl_sdr -f 800000000 -|./vectplot

