#include "compgeo.h"

Point2D points[MAXPOINTS];
Polygon2D polygons[100];
Dataset geometry[50];
size_t numpoints;
size_t numpolygons;
size_t numgeometry;

void UpdateGeometry(){
	for(size_t i = 0; i < numgeometry; i++){
		polygons[i] = ConvexHull(&points[geometry[i].start], geometry[i].end - geometry[i].start);
	}
}

void AddNewGeometry(){
	if(numgeometry){
		if(geometry[numgeometry-1].end - geometry[numgeometry-1].start == 0){
			return;
		}
		geometry[numgeometry-1].end = numpoints;
	}

	geometry[numgeometry].start = numpoints;
	geometry[numgeometry].end = numpoints;
	numgeometry++;
	numpolygons++;
}


double AngleTest(Point2D p1, Point2D p2, Point2D p3){
	int adx = p2.x - p1.x;
	int ady = p1.y - p2.y;
	int bdx = p3.x - p1.x;
	int bdy = p1.y - p3.y;

	// return atan2(bdy, bdx) - atan2(ady, adx);

	// return adx*((float)(ady)/adx - (float)(bdy)/bdx);

	// float a = sqrt(pow(adx, 2) + pow(ady, 2));
	// float b = sqrt(pow(bdx, 2) + pow(bdy, 2));

	// cross product:
	return (adx*bdy - ady*bdx);
}

void ClearGeometry(){
	numpoints = 0;
	numpolygons = 0;
	numgeometry = 0;
}

llPolygon2D* llRemoveElement(llPolygon2D* list, llPolygon2D* el){
	for(llPolygon2D* cur = list->next; cur != NULL; cur = cur->next){
		if(cur->next == el){
			cur->next = el->next;
			free(el);
			return cur->next;
		}
	}
	return NULL;
}

int amodb(int a, int b){
	int r = a%b;
	return r < 0 ? r + b : r;
}

void AddNewPoint(int x, int y){
	points[numpoints].x = x;
	points[numpoints].y = y;
	numpoints++;
	geometry[numgeometry-1].end++;
}
