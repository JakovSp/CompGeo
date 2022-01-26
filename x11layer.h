#ifndef X11LAYER
#define X11LAYER
#include <X11/X.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include "Draw.h"

typedef struct BACKBUFFER{
	Display* dpy;
	Window win;
	Pixmap pixmap;
	XImage* ximage;
	Visual* xvisual;
	GC gc;
	int screen;
	Framebuffer* framebuffer;
}Displaybuffer;

Displaybuffer* x11InitDisplayBuffer();
void x11RedrawDisplay(Displaybuffer*, int width, int height);
void x11FlipBackbuffer(Displaybuffer* offscreen);
void x11ClearGeometry();

#endif
