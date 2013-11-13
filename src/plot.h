// divisors for grid calcs
#define VGRID_XDIV 16
#define VGRID_YDIV 16
#define FGRID_XDIV 16
#define FGRID_YDIV 32

typedef struct {
  Display                 *dsp;
  Visual                  *vis;
  XSetWindowAttributes    frm_att;
  Window                  frm_win;
  XWindowAttributes       win_att;
  XFontStruct             *fontinfo;
  XGCValues               grv;
  GC                      gcxt;
  GC                      vectgrid_gc;
  GC                      fftgrid_gc;
	GC											label_gc;
  XEvent                  event;
  int                     depth;
} XVars;

void calcGrid(XSegment *,Coord *,int,int);
void setPlotEdges(int,int,Coord *,Coord *);
void buildWindow(XVars *);
void handleWinEvents(XVars *,Coord *,Coord *,XSegment *,XSegment *);
inline int x2screen(int,Coord *);
inline int y2screen(int,Coord *);

