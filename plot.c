#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include "defs.h"
#include "plot.h"

// startup window size
#define WINW 600
#define WINH 800

void setPlotEdges(int winw,int winh,Coord *vectp,Coord *fftp) {
  vectp->sx=5;
  vectp->sy=5;
  vectp->ex=winw-5;
  vectp->ey=winh-205; // fft will be 200px tall

  fftp->sx=5;
  fftp->sy=vectp->ey+5;
  fftp->ex=winw-5;
  fftp->ey=winh-5;
}

void buildWindow(XVars *xv) {
  xv->dsp  =XOpenDisplay(NULL);
  xv->vis  =DefaultVisual(xv->dsp,0);
  xv->depth=DefaultDepth(xv->dsp,0);

  xv->frm_att.background_pixel=XBlackPixel(xv->dsp,0);
  xv->frm_win=XCreateWindow(xv->dsp,XRootWindow(xv->dsp,0),
      0,0,WINW,WINH,5,xv->depth,InputOutput,xv->vis,
      CWBackPixel,&(xv->frm_att));

  XStoreName(xv->dsp,xv->frm_win,"Vectorplot v0.1");
  XSelectInput(xv->dsp,xv->frm_win,ExposureMask|StructureNotifyMask);

  // tell the wm we want to handle the close
  Atom wmClose=XInternAtom(xv->dsp,"WM_DELETE_WINDOW",True);
  XSetWMProtocols(xv->dsp,xv->frm_win,&wmClose,1);

  xv->fontinfo=XLoadQueryFont(xv->dsp,"10x20");
  xv->grv.font=xv->fontinfo->fid;
  xv->grv.foreground=XWhitePixel(xv->dsp,0);
  xv->gcxt=XCreateGC(xv->dsp,xv->frm_win,GCFont+GCForeground,&(xv->grv));

  // make colors
  Colormap colormap;
  XColor  vectgrid_col;
  XColor  fftgrid_col;

  colormap=DefaultColormap(xv->dsp,0);
  xv->vectgrid_gc=XCreateGC(xv->dsp,xv->frm_win,0,0);
  XParseColor(xv->dsp,colormap,"#FFFF00",&vectgrid_col);
  XAllocColor(xv->dsp,colormap,&vectgrid_col);
  XSetForeground(xv->dsp,xv->vectgrid_gc,vectgrid_col.pixel);

  colormap=DefaultColormap(xv->dsp,0);
  xv->fftgrid_gc=XCreateGC(xv->dsp,xv->frm_win,0,0);
  XParseColor(xv->dsp,colormap,"#00FF00",&fftgrid_col);
  XAllocColor(xv->dsp,colormap,&fftgrid_col);
  XSetForeground(xv->dsp,xv->fftgrid_gc,fftgrid_col.pixel);

  // finally, show the window
  XMapWindow(xv->dsp,xv->frm_win);
}

void handleWinEvents(XVars *xv,Coord *vp,Coord *fp,XSegment *vgrid,XSegment *fgrid) {
  XNextEvent(xv->dsp,(XEvent *)&(xv->event)); // blocking,flushes

  switch (xv->event.type) {
    case Expose: // on window change
      // nab window values
      XGetWindowAttributes(xv->dsp,xv->frm_win,&xv->win_att);

      setPlotEdges(xv->win_att.width,xv->win_att.height,vp,fp);
      calcGrid(vgrid,vp,VGRID_XDIV,VGRID_YDIV);
      calcGrid(fgrid,fp,FGRID_XDIV,FGRID_YDIV);

      break;
    case DestroyNotify:
      printf("Window destroyed, leaving.\n");
      break;
    case ClientMessage:
      need2leave();
      XDestroyWindow(xv->dsp,xv->frm_win);
      XCloseDisplay(xv->dsp);
      exit(0);
      break;
    default:
      break;
  }
}

inline int x2screen(int x,Coord *plot) {
  return (int)((x/HISTSIZE_F)*(plot->ex-plot->sx))+plot->sx;
}

inline int y2screen(int y,Coord *plot) {
  return (int)(plot->ey-((y/HISTSIZE_F)*(plot->ey-plot->sy)));
}

void calcGrid(XSegment *grid,Coord *plot,int divx,int divy) {
  int i,p=0;

  for (i=0;i<=HISTSIZE;i+=divx) {
      grid[p].x1=x2screen(i,plot);
      grid[p].y1=y2screen(0,plot);
      grid[p].x2=grid[p].x1;
      grid[p].y2=y2screen(HISTSIZE,plot);
      p++;
    }
    for (i=0;i<=HISTSIZE;i+=divy) {
      grid[p].x1=x2screen(0,plot);
      grid[p].y1=y2screen(i,plot);
      grid[p].x2=x2screen(HISTSIZE,plot);
      grid[p].y2=grid[p].y1;
      p++;
    }
}

