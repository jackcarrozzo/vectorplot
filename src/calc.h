typedef struct{float Re;float Im;} complex;
void fft(complex *,int,complex *);
inline float mag(complex);
float fft2float(complex *,int,float *);
uint16_t hist_2d(uint8_t *,int,uint16_t[HISTSIZE][HISTSIZE]);

