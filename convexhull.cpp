#include <cstdarg>
#include <float.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <x86_64-pc-linux-gnu/mpi.h>

#include "compgeo.h"

Polygon2D ConvexHull(Point2D* inputpoints, int npoints){
	Polygon2D ConvexPolygon;
	ConvexPolygon.points = (Point2D*)malloc(sizeof(Point2D)*npoints);
	memset(ConvexPolygon.points, 0, npoints*sizeof(Point2D));
	short* upper = (short*)malloc(sizeof(short)*npoints);
	memset(upper, 0, npoints*sizeof(short));
	int p = 0;

	SortPointsByX(inputpoints, npoints);

	// Upper hull:
	// add first starting point to the hull:
	// make tests by line segments
	int cur = 0;
	ConvexPolygon.points[0] = inputpoints[0];
	upper[0] = 1;
	cur++;
	// for each possible starting points:
	int start = 0;
	while(start < npoints - 1){
		int end = start+1;
		while(end < npoints){
			int test = 1;
			float direction = 0;
			for(int k = end+1; k < npoints; k++){
				// for each test decide where the test point lies:
				int adx = inputpoints[end].x - inputpoints[start].x;
				int ady = inputpoints[start].y - inputpoints[end].y;
				int bdx = inputpoints[k].x - inputpoints[start].x;
				int bdy = inputpoints[start].y - inputpoints[k].y;
				direction = (float)(ady)/adx - (float)(bdy)/bdx;
				if( adx >= 0 && bdx >= 0 && direction <= 0 ){
					// if a test fails, make the compared point new endpoint
					// and 'break' to test new endpoint;
					test = 0;
					end = k;
					break;
				}
			}
			// as soon as one endpoint passes all the tests make that endpoint
			// the next starting point;
			if(test){
				ConvexPolygon.points[cur] = inputpoints[end];
				upper[end] = 1;
				start = end;
				cur++;
				break;
			}
		}
	}
	
	// Lower hull
	// FIXME: often doesn't get constructed properly
	start = npoints-1;
	while(start >= 1){
		int end = start-1;
		while(end >= 0){
			if(upper[end]){
				end--;
				continue;
			}
			int test = 1;
			float direction = 0;
			for(int k = end-1; k >= 0; k--){
				int adx = inputpoints[end].x - inputpoints[start].x;
				int ady = inputpoints[start].y - inputpoints[end].y;
				int bdx = inputpoints[k].x - inputpoints[start].x;
				int bdy = inputpoints[start].y - inputpoints[k].y;
				direction = (float)(ady)/adx - (float)(bdy)/bdx;
				if( adx <= 0 && bdx <= 0 && direction <= 0 ){
					test = 0;
					end = k;
					break;
				}
			}
			if(test){
				ConvexPolygon.points[cur] = inputpoints[end];
				cur++;
				break;
			}
		}
		start = end;
	}

	free(upper);
	ConvexPolygon.numpoints = cur;
	return ConvexPolygon;
}

void ConvexUpperTangent(Polygon2D p1, Polygon2D p2, int& ta1, int& ta2){
	// Upper tangent
	// test left-top line segment
	for(int top = 0; top <= p1.numpoints-1; top=amodb(top+1,p1.numpoints)){
		int test = 1;
		int left = amodb((top-1),p1.numpoints);
		int right = amodb((top+1),p1.numpoints);
		ta2 = 0;
		double diff = 0;
		double r = DBL_MAX;
		// if left-top segment is covering *ALL* points in opposing polygon:
		for(int k = 0; k < p2.numpoints; k++){
			diff = Sine(p1.points[left], p1.points[top], p2.points[k]);
			if( diff > 0 ){
				test = 0;
				break;
			}
			if(abs(diff) < abs(r)){
				r = diff;
				ta2 = k;
			}
		}
		// if it fails, move to next configuration
		if(!test)
			continue;
		// test top-right line segment
		// if top-right segment is *NOT* covering *AT LEAST ONE* point in opposing polygon:
		test = 0;
		for(int k = 0; k < p2.numpoints; k++){
			if( Sine(p1.points[top], p1.points[right], p2.points[k]) > 0 ){
				test = 1;
				break;
			}
		}
		// if it fails, move to next configuration
		if(test){
			ta1 = top;
			break;
		}
	}

}

void ConvexLowerTangent(Polygon2D p1, Polygon2D p2, int& tb1, int& tb2){
	// Lower tangent
	for(int left = 1; left != 2; left=amodb((left-1),p1.numpoints)){
		int test = 1;
		int top = amodb((left-1),p1.numpoints);
		int right = amodb((left-2),p1.numpoints);
		double diff = 0;
		double r = DBL_MAX;
		for(int k = 0; k < p2.numpoints; k++){
			diff = Sine(p1.points[left], p1.points[top], p2.points[k]);
			if( diff < 0 ){
				test = 0;
				break;
			}
			if(abs(diff) < abs(r)){
				r = diff;
				tb2 = k;
			}
		}
		if(!test)
			continue;
		test = 0;
		for(int k = 0; k < p2.numpoints; k++){
			if( Sine(p1.points[top], p1.points[right], p2.points[k]) < 0 ){
				test = 1;
				break;
			}
		}
		if(test){
			tb1 = top;
			break;
		}
	}
}


// NOTE: Works correctly when convex polygons are intersecting at most twice;
// TODO: Create a separate method to cover all polygon intersection cases
Polygon2D ConnectConvex(Polygon2D p1, Polygon2D p2){
	int ta1, ta2, tb1, tb2;

	// NOTE: Order the start of p1 and p2 by x.
	// Necessary condition for commutativity
	Polygon2D MergedPolygon;
	if(p1.points[0].x > p2.points[0].x){
		Polygon2D temp = p1;
		p1 = p2;
		p2 = temp;
	}

	// NOTE: Construction of upper and lower tangent is independent.
	// Necessary condition for associativity
	// TODO: Parallel stitching hulls. Split the job on upper and lower
	// construction


	// Algorithm:
	// #1 Send SUBHULL signal
	// #2 Send TANGENT signal
	// #3 AsyncWait on TANGENT
	// #4 If AsyncWait is not completed: go first and send TANGENT signal
	//         Create Upper hull
	// else
	//         Create Lower hull and send TANGENT signal
	//         Finalize
	// #5 Construct Final hull and send SUBHULL signal

	ConvexUpperTangent(p1, p2, ta1, ta2);
	ConvexLowerTangent(p1, p2, tb1, tb2);

	int firsthull = ta1+1;
	int secondhull = 0;
	if(tb2){
		secondhull = tb2-ta2+1;
	} else {
		secondhull = p2.numpoints-ta2+1;
	}
	int thirdhull = 0;
	if(tb1)
		thirdhull = p1.numpoints-tb1;

	MergedPolygon.numpoints = firsthull + secondhull + thirdhull;
								
	MergedPolygon.points = (Point2D*)malloc(sizeof(Point2D)*MergedPolygon.numpoints);
	memset(MergedPolygon.points,0,sizeof(Point2D)*MergedPolygon.numpoints);
	for(int i = 0; i < firsthull; i++)
		MergedPolygon.points[i] = p1.points[i];
	for(int i = 0; i < secondhull; i++)
		MergedPolygon.points[firsthull+i] = p2.points[(ta2+i)%p2.numpoints];
	for(int i = 0; i < thirdhull; i++)
		MergedPolygon.points[firsthull+secondhull+i] = p1.points[tb1+i];
	
	return MergedPolygon;
}


void MergePolygons(Polygon2D p1, Polygon2D p2){
	Polygon2D newpolygon = ConnectConvex(p1, p2);
	// if(p1.points)
	// 	free(p1.points);
	// p1.points = NULL;
	// if(p2.points)
	// 	free(p2.points);
	// p2.points = NULL;

	// polygons[numpolygons-1] = newpolygon;
}

void p2pConvexHull(){
	int numdata;
	MPI_Recv(&numdata, 1, MPI_INT, MPI_MAIN_RANK, MPI_WORKER+mpi_rank-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	Point2D* inputpoints = NULL;

	while(numdata){
		MPI_Request sending[mpi_size-1][2];
		DebugPrint("Received job from Main Rank(%d)\n", MPI_MAIN_RANK);
		if(inputpoints){
			if(inputpoints)
				free(inputpoints);
			inputpoints = NULL;
		}
		inputpoints = (Point2D*)malloc(sizeof(Point2D)*numdata);

		DebugPrint("Receiving point data\n");
        MPI_Recv(inputpoints, numdata*sizeof(Point2D), MPI_BYTE, 0, MPI_WORKER+mpi_rank-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		DebugPrint("Received point data\n");

		// DebugPrint("Creating Convex Hull on %lu points\n", numdata);
		Polygon2D polygon = ConvexHull(inputpoints, numdata);

		DebugPrint("Sending new convex polygon\n");
		MPI_Ibsend(&polygon.numpoints, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &sending[mpi_rank-1][0]);
		DebugPrint("Sending %d data\n", polygon.numpoints);
		MPI_Ibsend(polygon.points, polygon.numpoints*sizeof(Point2D), MPI_BYTE, 0, 2, MPI_COMM_WORLD, &sending[mpi_rank-1][0]);
		MPI_Recv(&numdata, 1, MPI_INT, MPI_MAIN_RANK, MPI_WORKER+mpi_rank-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		DebugPrint("Received new task, clearing geometry\n", polygon.numpoints);
	}
	DebugPrint("Finalizing\n");
	if(inputpoints)
		free(inputpoints);
}

// TODO: Implement non-contiguous polygon stitching?
void p2pMainHull(int width, int height) {
	// randomize points across width and height
	struct timeval tv;
	int numdata = SAMPLE_SIZE;
	int widthspan = 0; int heightspan = 0;
	MPI_Request receiving[mpi_size-1];
	MPI_Request sending[mpi_size-1];

	gettimeofday(&tv, NULL);
	unsigned long long millisecondsSinceEpoch =
		(unsigned long long)(tv.tv_sec) * 1000 +
		(unsigned long long)(tv.tv_usec) / 1000;
	srand(millisecondsSinceEpoch);

	DebugPrint("Generating random points\n");
	for(int i = 0; i < SAMPLE_SIZE; i++){
		points[i].x = 200 + rand()%(width-400);
		points[i].y = 150 + rand()%(height-300);
		numpoints++;
		adding_end=numpoints-1;
	}
	widthspan = SAMPLE_SIZE/mpi_size;

	SortPointsByX(points, SAMPLE_SIZE);

	// Initial hull
	for(int i = 1; i<mpi_size; i++){
		// Split data to all the ranks
		DebugPrint("Sending %d data to Rank: %d\n", widthspan, i);
		// QUERY: Immediate + Buffered send? 
		MPI_Ibsend(&widthspan, 1, MPI_INT, i, MPI_WORKER+i-1, MPI_COMM_WORLD, &sending[i-1]);
		MPI_Ibsend(&points[widthspan*i], widthspan*sizeof(Point2D), MPI_BYTE, i, MPI_WORKER+i-1, MPI_COMM_WORLD, &sending[i-1]);
	}

	for(int i = 1; i<mpi_size; i++){
		DebugPrint("Waiting for new polygon data from Rank: %d\n", i);
		MPI_Irecv(&(polygons[numpolygons+i].numpoints), 1, MPI_INT, i, 1, MPI_COMM_WORLD, &receiving[i-1]); 
	}

	polygons[numpolygons] = ConvexHull(&points[0], widthspan);

	// TODO: Try implementing MPI_ANY_TAG Receive
	for(int i = 1; i<mpi_size; i++){
		DebugPrint("Waiting for new polygon data from Rank: %d\n", i);
		MPI_Wait(&receiving[i-1], MPI_STATUS_IGNORE);
		polygons[numpolygons+i].points = (Point2D*)malloc(polygons[numpolygons+i].numpoints*sizeof(Point2D));
		MPI_Recv(polygons[numpolygons+i].points, polygons[numpolygons+i].numpoints*sizeof(Point2D), MPI_BYTE, i, 2, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		DebugPrint("Received %d new polygon data from Rank: %d\n", polygons[numpolygons+i].numpoints, i);
		polygons[numpolygons] = ConnectConvex(polygons[numpolygons], polygons[numpolygons+i]);
		DebugPrint("Merged with Rank %d polygon\n", i);
	}

	numpolygons+=mpi_size;
	// numpolygons++;

}

// void collConvexHull(){
// 	int numdata = 0;
// 	Point2D* inputdata;
// 	MPI_Recv(&numdata, 1, MPI_INT, MPI_MAIN_RANK, MPI_WORKER+mpi_rank-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
// 	MPI_Scatter(points, numdata*sizeof(Point2D), MPI_BYTE, inputdata, numdata*sizeof(Point2D), MPI_BYTE, mpi_rank, MPI_COMM_WORLD);
// 	if(mpi_rank != 0){
// 	}
// 	MPI_Gather(points, widthspan*sizeof(Point2D), MPI_BYTE, inputpoints, widthspan*sizeof(Point2D), MPI_BYTE, 0, MPI_COMM_WORLD);
// 	MPI_Wait();
// }

// void collMainHull(int width, int height){
// 	// randomize points across width and height
// 	int numdata = SAMPLE_SIZE;
// 	int widthspan = 0; int heightspan = 0;
// 	MPI_Request receiving[mpi_size-1];
// 	MPI_Request sending[mpi_size-1];

// 	DebugPrint("Generating random points\n");
// 	srand(time(NULL));
// 	for(int i = 0; i < SAMPLE_SIZE; i++){
// 		points[i].x = 200 + rand()%(width-400);
// 		points[i].y = 150 + rand()%(height-300);
// 		numpoints++;
// 		adding_end=numpoints-1;
// 	}
// 	widthspan = SAMPLE_SIZE/mpi_size;

// 	SortPointsByX(points, SAMPLE_SIZE);


// 	MPI_Scatter(points, widthspan*sizeof(Point2D), MPI_BYTE, inputpoints, widthspan*sizeof(Point2D), MPI_BYTE, 0, MPI_COMM_WORLD);
// 	MPI_Op_commutative(MPI_Op op, int *commute)

// 	// NOTE: Connecting convex polygons with tangents is commutative and associative, it can be
// 	// used as reduce operation effectively.
// 	// Since it is necessary to have at least two polygons constructed to call
// 	// ConnectConvex (), the trigger for ANY_TAG or MPI_Test on ANY_RANK, could be
// 	// used to initiate the function.
// 	// QUERY: Data from single rank polygons could be broadcast to all?
// }
