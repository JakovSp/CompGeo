#ifndef COMPGEO
#define COMPGEO
#include <stdlib.h>
#include <math.h>

#define MAXPOINTS 5000

typedef struct POINT2D{
	int x;
	int y;
}Point2D;

typedef struct MESH Mesh;
typedef struct LL_POLYGON llPolygon2D;

struct LL_POLYGON{
	Point2D point;
	llPolygon2D* next;
};

struct MESH{
	Point2D point;
	Mesh* points;
};

typedef struct POLYGON2D{
	Point2D* points;
	int numpoints;
} Polygon2D;

typedef struct DATASET{
	size_t start;
	size_t end;
}Dataset;

extern Point2D points[MAXPOINTS];
extern Polygon2D polygons[100];
extern Dataset geometry[50];
extern size_t numpoints;
extern size_t numpolygons;
extern size_t numgeometry;

void UpdateGeometry();
void AddNewGeometry();
double AngleTest(Point2D p1, Point2D p2, Point2D p3);
void ClearGeometry();
llPolygon2D *llRemoveElement(llPolygon2D *list, llPolygon2D *el);
int amodb(int a, int b);
void AddNewPoint(int x, int y);

Polygon2D ConvexHull(Point2D* inputpoints, int npoints);
void MergePolygon();

#endif
