vectorplot
==========

###Vector and spectrum analyzer functionality for the RTL_SDR


Currently, data is taken from the rtl_sdr binary via stdout:

````rtl_sdr -f 800000000 - | ./vectplot````

###What currently works:

- Spectrum Analyzer
- Vector Analyzer (IQ constelation plotting)

###To do:

- Labels
- Spectrum analyzer peak hold
- Constelation plot slow decay
- Freq error and sync
- Network support (receive samples from rtl_tcp)
- RTL_SDR libary support (needed for gain and freq control)

