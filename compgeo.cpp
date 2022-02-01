#include "compgeo.h"

Point2D* points = NULL;
Line2D* lines = NULL;
Polygon2D* polygons = NULL;
size_t numpoints = 0;
size_t numlines = 0;
size_t numpolygons = 0;

geotype adding=none;
size_t adding_start=0;
size_t adding_end=0;

void AddNewPoint(int x, int y){
	points[numpoints].x = x;
	points[numpoints].y = y;
	numpoints++;
	adding_end=numpoints-1;
}

void AddNewLine(size_t a, size_t b){
	lines[numlines].a = points[a];
	lines[numlines].b = points[b];
	numlines++;
}

void AddNewConvexPolygon(size_t start, size_t end){
	polygons[numpolygons] = ConvexHull(&points[start], end-start+1);
	numpolygons++;
}

double Sine(Point2D p1, Point2D p2, Point2D p3){
	int adx = p2.x - p1.x;
	int ady = p1.y - p2.y;
	int bdx = p3.x - p1.x;
	int bdy = p1.y - p3.y;
	return atan2(bdy, bdx) - atan2(ady, adx);
}

double Cross(Point2D p1, Point2D p2, Point2D p3){
	int adx = p2.x - p1.x;
	int ady = p1.y - p2.y;
	int bdx = p3.x - p1.x;
	int bdy = p1.y - p3.y;
	return (adx*bdy - ady*bdx);
}

void ClearGeometry(){
	memset(points, 0, numpoints*sizeof(Point2D));
	memset(lines, 0, numlines*sizeof(Line2D));
	for(int i = 0; i < numpolygons; i++){
		if(polygons[i].points)
			free( polygons[i].points );
	}
	memset(polygons, 0, numpolygons*sizeof(Polygon2D));

	numpoints = 0;
	numlines = 0;
	numpolygons = 0;
	adding_start = 0;
	adding_end = 0;
}

int amodb(int a, int b){
	int r = a%b;
	return r < 0 ? r + b : r;
}

void AddGeometry(enum geotype newgeo){
	switch(newgeo){
	case line:
		if(adding_end){
			adding_start=adding_end+1;
			adding_end++;
		}
		AddNewLine(adding_start, adding_end);
		adding=newgeo;
		break;
	case polygon:
		adding_start=adding_end+1;
		adding_end++;
		AddNewConvexPolygon(adding_start, adding_end);
		adding=newgeo;
		break;
	case point:
	case none:
	default:
		return;
		break;
	}	
}

void UpdateGeometry(){
	switch(adding){
	case line:
		lines[numlines-1].b = points[adding_end];
		break;
	case polygon:
		polygons[numpolygons-1] = ConvexHull(&points[adding_start], adding_end - adding_start + 1);
		break;
	default:
		break;
	}
}

void SortPointsByX(Point2D* pointsdata, int npoints){
	int swapped = 1;
	while(swapped){
		swapped = 0;
		for(int i = 0; i < npoints-1; i++){
			if(pointsdata[i].x > pointsdata[i+1].x){
				Point2D temp = pointsdata[i+1];
				pointsdata[i+1] = pointsdata[i];
				pointsdata[i] = temp;
				swapped = 1;
			}
		}
	}
}
