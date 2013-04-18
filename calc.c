#include <stdint.h>
#include <math.h>
#include "defs.h"
#include "calc.h"

/*
   Cooley-Tukey, based on 
http://www.math.wustl.edu/~victor/mfmm/fourier/fft.c

fft(v,N):
- [0] If N==1 then return.
- [1] For k = 0 to N/2-1, let ve[k] = v[2*k]
- [2] Compute fft(ve, N/2);
- [3] For k = 0 to N/2-1, let vo[k] = v[2*k+1]
- [4] Compute fft(vo, N/2);
- [5] For m = 0 to N/2-1, do [6] through [9]
- [6]   Let w.re = cos(2*PI*m/N)
- [7]   Let w.im = -sin(2*PI*m/N)
- [8]   Let v[m] = ve[m] + w*vo[m]
- [9]   Let v[m+N/2] = ve[m] - w*vo[m]
 */
void fft(complex *v,int n,complex *tmp) {
  if (1==n) return;

  int k,m;
  complex z,w,*vo,*ve;
  ve=tmp;
  vo=tmp+n/2;

  for(k=0;k<n/2;k++) {
    ve[k]=v[2*k];
    vo[k]=v[2*k+1];
  }

  fft(ve,n/2,v); // evens
  fft(vo,n/2,v); // odds

  for(m=0; m<n/2; m++) {
    w.Re=cos(TWOPI*m/(double)n);
    w.Im=-sin(TWOPI*m/(double)n);
    z.Re=w.Re*vo[m].Re-w.Im*vo[m].Im;
    z.Im=w.Re*vo[m].Im+w.Im*vo[m].Re;
    v[m].Re=ve[m].Re+z.Re;
    v[m].Im=ve[m].Im+z.Im;
    v[m+n/2].Re=ve[m].Re-z.Re;
    v[m+n/2].Im=ve[m].Im-z.Im;
  }
}

// vector magnitude (a^2 + b^2) ^1/2
inline float mag(complex v) {
  return sqrt((v.Re*v.Re)+(v.Im*v.Im));
}

// takes the mag() of each bin, AND swaps the
// left and right halves (ie, 0-centered)
float fft2float(complex *v,int n,float *mags) {
  int i;
  int center=n/2;
  float max=0;

  for (i=0;i<center;i++) {
    mags[i+center]=mag(v[i]);
    if (mags[i+center]>max) max=mags[i+center];
  }
  for (i=center;i<n;i++) {
    mags[i-center]=mag(v[i]);
    if (mags[i-center]>max) max=mags[i-center];
  }

  return max;
}

// calculate the 2d histogram of the samples,
// and return the max value
uint16_t hist_2d(uint8_t *vals,int n,uint16_t plot[HISTSIZE][HISTSIZE]) {
  int i,j;
  uint16_t max=0;
  uint8_t thisi=0,thisq=0;

  // zero the plot
  for (i=0;i<HISTSIZE;i++) { // 256 because the IQ vals we get are uint8_t
    for (j=0;j<HISTSIZE;j++) {
      plot[i][j]=0;
    }
  }

  n*=2; // pairs
  while(i<n) {
    thisi=vals[i++];
    thisq=vals[i++];

    plot[thisi][thisq]++;
    if (plot[thisi][thisq]>max) max=plot[thisi][thisq];
  }

  return max;
}


