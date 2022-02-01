#ifndef COMPGEO
#define COMPGEO
#include <stdlib.h>
#include <math.h>

#include <openmpi/ompi/mpi/cxx/mpicxx.h>
extern int mpi_rank;
extern int mpi_size;
#define MPI_MAIN_RANK 0
#define MPI_WORKER 100

#define MAXPOINTS 1000
#define SAMPLE_SIZE 500

enum geotype{
	none=0, point=1, line=2, circle=3, polygon=4
};

typedef struct POINT2D{
	int x;
	int y;
}Point2D;

typedef struct LL_POLYGON llPolygon2D;

struct LL_POLYGON{
	Point2D point;
	llPolygon2D* next;
};

typedef struct POLYGON2D{
	Point2D* points;
	int numpoints;
}Polygon2D;

typedef struct LINE2D{
	Point2D a;
	Point2D b;
}Line2D;

typedef struct CIRCLE2D{
	Point2D center;
	float radius;
}Circle2D;

extern Point2D* points;
extern Line2D* lines;
extern Polygon2D* polygons;
extern size_t numpoints;
extern size_t numlines;
extern size_t numpolygons;

extern enum geotype adding;
extern size_t adding_start;
extern size_t adding_end;

void AddNewPoint(int x, int y);
void AddNewLine(size_t a, size_t b);
void AddNewConvexPolygon(size_t start, size_t end);
double Sine(Point2D p1, Point2D p2, Point2D p3);
double Cross(Point2D p1, Point2D p2, Point2D p3);
void ClearGeometry();
void UpdateGeometry();
void AddGeometry(enum geotype);
int amodb(int a, int b);

void MergePolygons(Polygon2D p1, Polygon2D p2);
Polygon2D ConvexHull(Point2D* inputpoints, int npoints);
void MPI_ConvexHull();

inline void DebugPrint(const char *__restrict format, ...){
#ifdef DEBUG
	printf("Rank: %d ", mpi_rank);
	va_list argptr;
	va_start(argptr, format);
	vprintf(format, argptr);
	va_end(argptr);
#endif
}

// NOTE: simple text replace to call valgrind wrappers
#ifdef DEBUG
#define MPI_Send PMPI_Send
#define MPI_Recv PMPI_Recv
#endif

#endif
