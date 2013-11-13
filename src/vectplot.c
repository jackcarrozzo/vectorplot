#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <X11/Xlib.h>

#include "defs.h"
#include "calc.h"
#include "plot.h"

// defaults
#define DEF_CHUNKSIZE  (16*16384) // for reads
#define DEF_WINSIZE    16384      // fft and hist window
#define DEF_SAMPLERATE 2048000    // for freq calcs

#define DEBUG_TIME 0 							// print how long we block at fread()

void need2leave() {
	printf("Exiting cleanly.\n");
	// clean up?
}

int main(int argc,char **argv) {
	FILE *fp;

	if (argc>1) {
		if(!strcmp(argv[1],"-")) {
			fp=stdin;
		} else {
			fp=fopen(argv[1],"rb");
			if (NULL==fp) {
				fprintf(stderr, "Unable to open '%s': %s\n",argv[1],strerror(errno));
				exit(1);
			}
		}
	} else {
		if (isatty(fileno(stdin))) {
			printf("You either need to pipe in samples, or pass a filename as an argument.\n");
			exit(1);
		}

		printf(">>> Reading samples from stdin...\n");
		fp=stdin;
	}

	int winsize=DEF_WINSIZE;
  int samplerate=DEF_SAMPLERATE;

	uint8_t samples[DEF_CHUNKSIZE];
	complex vs[winsize],tmp[winsize];
	float mags[winsize];

	uint16_t histo[HISTSIZE][HISTSIZE];
	uint16_t histmax;

	struct timeval t;
	int u_start,u_diff;

	Coord vectplot;
	Coord fftplot;

	XVars xv;
	buildWindow(&xv);

	XSegment vectgrid[17+17]; // 17 lines per side
	XSegment fftgrid [17+9];  // 17 across, 9 up

	// set our initial positions (these update when the window resizes)
	XGetWindowAttributes(xv.dsp,xv.frm_win,&(xv.win_att));
	setPlotEdges(xv.win_att.width,xv.win_att.height,&vectplot,&fftplot);
	calcGrid(vectgrid,&vectplot,VGRID_XDIV,VGRID_YDIV);
  calcGrid(fftgrid,&fftplot,FGRID_XDIV,FGRID_YDIV);

	int i,j,c,n;
	float fftmax;

	XPoint histpoints[HISTSIZE*HISTSIZE]; 
	XPoint fftpoints[winsize];
	XPoint fftmaxhold[winsize]; // will only use this many if win is very wide
	char reset_maxhold=1;

   // reset and allocate
  for (i=0;i<(fftplot.ex-fftplot.sx);i++) {
    fftmaxhold[i].y=fftplot.ey;
    fftmaxhold[i].x=i;
  }
  int num_holdpts=fftplot.ex-fftplot.sx;

	while(1) {
		while (XEventsQueued(xv.dsp,QueuedAfterFlush)) 
			handleWinEvents(&xv,&vectplot,&fftplot,vectgrid,fftgrid);
		XClearWindow(xv.dsp,xv.frm_win);
		XDrawSegments(xv.dsp,xv.frm_win,xv.vectgrid_gc,
			vectgrid,sizeof(vectgrid)/sizeof(vectgrid[0]));
		XDrawSegments(xv.dsp,xv.frm_win,xv.fftgrid_gc,
			fftgrid,sizeof(fftgrid)/sizeof(fftgrid[0]));		

		// TODO: rm magic numbers, only recalc on window change

		// vect plot details -----
		XDrawArc(xv.dsp,xv.frm_win,xv.label_gc,
      ((vectplot.sx+vectplot.ex)/2)-4,
      ((vectplot.sy+vectplot.ey)/2)-4,
      8,8,
      0,23040);

		XDrawString(xv.dsp,xv.frm_win,xv.label_gc,
			vectplot.ex-15,
			((vectplot.ey-vectplot.sy)/2)+vectplot.sy-5,
			"+I",2);

    XDrawString(xv.dsp,xv.frm_win,xv.label_gc,
      5+(vectplot.sx+vectplot.ex)/2,
      vectplot.sy+15,
      "+Q",2);
		// ----------------------

		// fft plot details -----
		XDrawArc(xv.dsp,xv.frm_win,xv.label_gc,
      ((fftplot.sx+fftplot.ex)/2)-4,
      fftplot.ey-4,
      8,8,
      0,23040);	

		XDrawString(xv.dsp,xv.frm_win,xv.label_gc,
			fftplot.ex-50,
			fftplot.ey-1,
			"+1 MHz >",8);

		XDrawString(xv.dsp,xv.frm_win,xv.label_gc,
      fftplot.sx+3,
      fftplot.ey-1,
      "< -1 MHz",8);

		// ----------------------

		if (DEBUG_TIME) {
			gettimeofday(&t,NULL);
			u_start=t.tv_usec;
		}

		if (!fread(samples,1,DEF_CHUNKSIZE,fp)) {
			printf("fread() 0\n");
			exit(1);
		}

		if (DEBUG_TIME) {
			gettimeofday(&t,NULL);
			if ((u_diff=t.tv_usec-u_start)<0) u_diff+=1000000; // handle wrap
			printf("fread() %d us\n",u_diff);
		}

		// TODO: consider *=0.008, from http://sdr.osmocom.org/trac/wiki/rtl-sdr
		// we only take the first winsize samples of the chunk
		c=0;
		for (i=0;i<winsize;i++) {
			vs[i].Re=(float)(samples[c++]-((HISTSIZE/2)-1)); // center on 0
			vs[i].Im=(float)(samples[c++]-((HISTSIZE/2)-1));
		}

		fft(vs,winsize,tmp);
		fftmax=fft2float(vs,winsize,mags);
		histmax=hist_2d(samples,winsize,histo);

		// since X11 is a hassle with colors, it's harder to shade the vector 
		// histogram based on number of hits. currently, this just shows a
		// sample on the plot if it's nonzero
		c=0;
		for (i=0;i<HISTSIZE;i++) {
			for (j=0;j<HISTSIZE;j++) {
				if (histo[i][j]!=0) {
					histpoints[c].x=x2screen(i,&vectplot);
					histpoints[c].y=y2screen(j,&vectplot);
					c++;
				}
			}
		}
		
		XDrawPoints(xv.dsp,xv.frm_win,xv.gcxt,histpoints,c,CoordModeOrigin);

		// log base fftmax of fft = log(fft)/log(fftmax)
		fftmax=log(fftmax);	
		for (i=0;i<winsize;i++) {
			fftpoints[i].x=x2screen(1+(int)((HISTSIZE-1)*i/winsize),&fftplot);
			fftpoints[i].y=y2screen((int)((HISTSIZE-1)*log(mags[i])/fftmax),&fftplot);

			if (reset_maxhold) {
				//fftmaxhold[i].y=fftpoints[i].y;
				reset_maxhold=0;
			} else {
				// less than because we're in px
				if (fftpoints[i].y<fftmaxhold[fftpoints[i].x].y) 
					fftmaxhold[fftpoints[i].x].y=fftpoints[i].y;
			}
		}
		XDrawLines(xv.dsp,xv.frm_win,xv.gcxt,fftpoints,
				sizeof(fftpoints)/sizeof(fftpoints[0]),CoordModeOrigin);

		// TODO: don't draw max hold points that overlap the bottom gridline
		XDrawPoints(xv.dsp,xv.frm_win,xv.label_gc,fftmaxhold,num_holdpts,CoordModeOrigin);

	}

	fclose(fp);
	return 0;
}
