Vector and spectrum analyzer functionality for the RTL_SDR

Currently, data is taken from the rtl_sdr binary via stdout:

	rtl_sdr -f 800000000 - | ./vectplot

Example output:

	jackc@dev0 ~/Projects/vectorplot $ rtl_sdr -f 800000000 -|./vectplot 
	>>> Reading samples from stdin...
	Found 1 device(s):
	  0:  Generic, RTL2832U, SN: 77771111153705700

	Using device 0: Generic RTL2832U
	Found Elonics E4000 tuner
	Tuned to 800000000 Hz.
	Reading samples in async mode...
	^CSignal caught, exiting!

	User cancel, exiting...

Output from rtl_sdr is passed through to the console, since it's writing
samples to stdin and info to stderr.

What currently works:

- Spectrum Analyzer
- Vector Analyzer (IQ constelation plotting)

To do:

- Spectrum analyzer peak hold
- Constelation plot slow decay
- Freq error and clock sync
- Network support (receive samples from rtl_tcp)
- RTL_SDR libary support (needed for gain and freq control)

