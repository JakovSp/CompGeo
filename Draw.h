#ifndef DRAW
#define DRAW
#include "compgeo.h"

#define clip(x,lower,upper) (x < lower ? lower : (x > upper ? upper : x));

typedef struct FRAMEBUFFER{
	void* data;
	int width;
	int height;
	int depth;
}Framebuffer;

void Draw(Framebuffer* framebuffer);
void DrawLine(Framebuffer* drawingbuffer, int x1, int y1, int x2, int y2);
void DrawPoint(Framebuffer* drawingbuffer, int x, int y);
void DrawPolygon(Framebuffer* drawbuffer, Polygon2D polygon);

#endif
