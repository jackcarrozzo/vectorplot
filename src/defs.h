// because the samples we get from the RTL are chars,
// everything expects values from 0-255. this def is
// here to remove the magic numbers, but other sizes
// probably wont work.
#define HISTSIZE   256
#define HISTSIZE_F 256.0f

#define TWOPI     6.28318530717958647692528676655900576

typedef struct {
  int sx;
  int sy;
  int ex;
  int ey;
} Coord;
