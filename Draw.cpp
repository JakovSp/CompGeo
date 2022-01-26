#include <float.h>
#include <stdlib.h>
#include "Draw.h"

void PutPixel(Framebuffer* fb, int x, int y){
	int width = fb->width;
	int height = fb->height;
	x = clip(x,0,width);
	y = clip(y,0,height);
	int offset = width*4*y + x*4;
	u_int8_t* pixel = (u_int8_t*)(fb->data)+offset;
	*pixel = 0xFF;
	*(pixel + 1) = 0xFF;
	*(pixel + 2) = 0xFF;
	*(pixel + 3) = 0xFF;
}

void DrawLine(Framebuffer* drawingbuffer, int x1, int y1, int x2, int y2){
	// implement cut off point when endpoints lie outside of drawing area?
	int x=x1,y=y1;
	int sx,sy;
	int dx, dy;
	int error;
	dx = abs(x2-x1);
	dy = abs(y2-y1);
	sx = (x2 > x1) ? 1 : -1;
	sy = (y2 > y1) ? 1 : -1;

	if( dx > dy){
		error = 2*dy - dx;
		while(x != x2){
			PutPixel(drawingbuffer, x, y);
			x+=sx;
			error += dy;
			if(error > 0){
				error -= dx;
				y+=sy;
			}
		}
	} else{
		error = 2*dx - dy;
		while(y != y2){
			PutPixel(drawingbuffer, x, y);
			y+=sy;
			error += dx;
			if(error > 0){
				error -= dy;
				x+=sx;
			}
		}
	}
	PutPixel(drawingbuffer, x, y);
}

void DrawPoint(Framebuffer* drawingbuffer, int x, int y){
	PutPixel(drawingbuffer, x, y);
	PutPixel(drawingbuffer, x, y+1);
	PutPixel(drawingbuffer, x, y-1);
	PutPixel(drawingbuffer, x-1, y);
	PutPixel(drawingbuffer, x+1, y);
}

void DrawPolygon(Framebuffer* drawbuffer, Polygon2D polygon){
	if(polygon.numpoints >= 2){
		for(int i = 0; i < polygon.numpoints-1; i++){
			DrawLine(drawbuffer,polygon.points[i].x,polygon.points[i].y,polygon.points[i+1].x,polygon.points[i+1].y);
		}
			DrawLine(drawbuffer,
					 polygon.points[polygon.numpoints-1].x,
					 polygon.points[polygon.numpoints-1].y,
					 polygon.points[0].x,polygon.points[0].y);
	}
}

void Draw(Framebuffer* framebuffer){
	for(size_t i = 0; i < numpoints; i++){
		DrawPoint(framebuffer, points[i].x, points[i].y);
	}

	for(size_t i = 0; i < numpolygons; i++){
		DrawPolygon(framebuffer, polygons[i]);
	}
}
