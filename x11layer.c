#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "compgeo.h"
#include "x11layer.h"

static int running = 1;


void ClearScreen(Framebuffer* fb, int clscolor){
	memset(fb->data, clscolor, fb->width*fb->height*4);
}

void x11RedrawDisplay(Displaybuffer* displaybuffer, int width, int height){
	XExposeEvent expose;
	expose.type = Expose;
	expose.send_event = True;
	expose.display = displaybuffer->dpy;
	expose.window = displaybuffer->win;
	expose.x = 0; expose.y = 0;
	expose.width = width;
	expose.height = height;
	XSendEvent(displaybuffer->dpy, displaybuffer->win, 1, ExposureMask, (XEvent*)&expose);
}

void x11FlipBackbuffer(Displaybuffer* offscreen){
	XPutImage(offscreen->dpy, offscreen->pixmap, offscreen->gc, offscreen->ximage, 0, 0, 0, 0, offscreen->framebuffer->width, offscreen->framebuffer->height);
	XCopyArea(offscreen->dpy, offscreen->pixmap, offscreen->win, offscreen->gc, 0, 0, offscreen->framebuffer->width, offscreen->framebuffer->height, 0, 0);
}

void ResizeBackbuffer(Displaybuffer* offscreen, int width, int height, int depth){
	if(offscreen->framebuffer->data){
		XDestroyImage(offscreen->ximage);
		XFreePixmap(offscreen->dpy, offscreen->pixmap);
	}


	offscreen->framebuffer->width = width;
	offscreen->framebuffer->height = height;
	offscreen->framebuffer->depth = depth;

	offscreen->framebuffer->data = (int*)malloc(width*height*4);
	memset(offscreen->framebuffer->data, 0, width*height*4);
	offscreen->pixmap = XCreatePixmap(offscreen->dpy, offscreen->win, width, height, depth);
	offscreen->ximage = XCreateImage(offscreen->dpy, offscreen->xvisual, depth, ZPixmap, 0, (char*)offscreen->framebuffer->data, width, height, 32, 0);

}

Displaybuffer* x11InitDisplayBuffer(){
	Display* dpy;
	int screen;
	Window win;
	XVisualInfo xvinfo;
	XWindowAttributes attrs;
	int height, width;

	Displaybuffer* mainbuffer = (Displaybuffer*)malloc(sizeof(Displaybuffer));
	mainbuffer->framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer));

	dpy = XOpenDisplay(NULL);
	screen = DefaultScreen(dpy);

	win = XCreateWindow(dpy, RootWindow(dpy, screen), 10, 10, 500, 500, 1,24,InputOutput, CopyFromParent, 0, NULL);
	XSelectInput(dpy, win, ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask | StructureNotifyMask );
	XMapWindow(dpy, win);
 
	XGetWindowAttributes(dpy, win, &attrs);
	width = attrs.width;
	height = attrs.height;

	XMatchVisualInfo(dpy, screen, 24, DirectColor, &xvinfo);
	mainbuffer->xvisual = xvinfo.visual;
	mainbuffer->dpy = dpy;
	mainbuffer->screen = screen;
	mainbuffer->win = win;
	
	mainbuffer->gc = DefaultGC(mainbuffer->dpy, mainbuffer->screen);
	XGetWindowAttributes(dpy, win, &attrs);
	height = attrs.height;
	width = attrs.width;

	ResizeBackbuffer(mainbuffer, width, height, 24);
	
	return mainbuffer;
}

int main(void) {
	XEvent e;
	Displaybuffer* offscreen;
	int width, height;

	offscreen = x11InitDisplayBuffer();

	ClearGeometry();
	while (running) {
		XNextEvent(offscreen->dpy, &e);
		switch (e.type) {
			case Expose:
				Draw(offscreen->framebuffer);
				x11FlipBackbuffer(offscreen);
				XFlush(offscreen->dpy);
				break;
			case KeyPress:
				if(e.xkey.keycode == XKeysymToKeycode(offscreen->dpy,XK_M)){
					if(numpolygons >= 2) {
						MergePolygons(polygons[numpolygons-2],polygons[numpolygons-1]);
						ClearScreen(offscreen->framebuffer, 0);
						x11RedrawDisplay(offscreen, width, height);
					}
				}
				if(e.xkey.keycode == XKeysymToKeycode(offscreen->dpy,XK_C)){
					AddGeometry(polygon);
				}
				if(e.xkey.keycode == XKeysymToKeycode(offscreen->dpy,XK_L)){
					AddGeometry(line);
				}
				if(e.xkey.keycode == XKeysymToKeycode(offscreen->dpy,XK_Escape))
					running = 0;
				if(e.xkey.keycode == XKeysymToKeycode(offscreen->dpy,XK_space)){
                    ClearGeometry();
					ClearScreen(offscreen->framebuffer, 0);
					x11RedrawDisplay(offscreen, width, height);
				}
				break;
			case MotionNotify:
				break;
			case ButtonPress:
				AddNewPoint(e.xbutton.x,e.xbutton.y);
				ClearScreen(offscreen->framebuffer, 0);
				UpdateGeometry();
				x11RedrawDisplay(offscreen, width, height);
				break;
			case ConfigureNotify:
				height = e.xconfigure.height;
				width = e.xconfigure.width;
				ResizeBackbuffer(offscreen, width, height, 24);
				x11RedrawDisplay(offscreen, width, height);
				break;
			default: 
				break;
		}
	}

	XDestroyImage(offscreen->ximage);
	XFreePixmap(offscreen->dpy, offscreen->pixmap);
	XCloseDisplay(offscreen->dpy);
	/* free(offscreen->framebuffer); */
	/* free(offscreen); */
	return 0;
}
