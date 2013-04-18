all:
	gcc -o vectplot calc.c plot.c vectplot.c -lm -lX11 -O3
clean:
	rm -f vectplot
run:
	rtl_sdr -f 800000000 -|./vectplot

