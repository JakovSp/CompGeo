#include <float.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include "compgeo.h"

Polygon2D ConvexHull(Point2D* inputpoints, int npoints){
	Polygon2D ConvexPolygon;
	ConvexPolygon.points = (Point2D*)malloc(sizeof(Point2D)*npoints);
	memset(ConvexPolygon.points, 0, npoints*sizeof(Point2D));
	short* upper = (short*)malloc(sizeof(short)*npoints);
	memset(upper, 0, npoints*sizeof(short));

	// sort by x
	int swapped = 1;
	while(swapped){
		swapped = 0;
		for(int i = 0; i < npoints-1; i++){
			if(inputpoints[i].x > inputpoints[i+1].x){
				Point2D temp = inputpoints[i+1];
				inputpoints[i+1] = inputpoints[i];
				inputpoints[i] = temp;
				swapped = 1;
			}
		}
	}

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
	
	//Lower hull:
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
				// for each test decide where the test point lies:
				int adx = inputpoints[end].x - inputpoints[start].x;
				int ady = inputpoints[start].y - inputpoints[end].y;
				int bdx = inputpoints[k].x - inputpoints[start].x;
				int bdy = inputpoints[start].y - inputpoints[k].y;
				direction = (float)(ady)/adx - (float)(bdy)/bdx;
				if( adx <= 0 && bdx <= 0 && direction <= 0 ){
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

Polygon2D ConnectConvex(Polygon2D p1, Polygon2D p2){
	// assume p1 and p2 are convex
	Polygon2D ConvexPolygon;
	int ta1, ta2;
	int tb1, tb2;

	if(p1.points[0].x > p2.points[0].x){
		Polygon2D temp = p1;
		p1 = p2;
		p2 = temp;
	}

	// Upper tangent
	// test left-top line segment
	for(int top = 0; top <= p1.numpoints-1; top=amodb(top+1,p1.numpoints)){
		int test = 1;
		int left = amodb((top-1),p1.numpoints);
		int right = amodb((top+1),p1.numpoints);
		tb2 = 0;
		double diff = 0;
		double r = DBL_MAX;
		// if left-top segment is covering ALL points in opposing polygon:
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
		// if top-right segment is NOT covering AT LEAST ONE point in opposing polygon:
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

	int firsthull = ta1+1;
	int secondhull = 0;
	if(tb2){
		secondhull = tb2-ta2+1;
	} else {
		secondhull = p2.numpoints-ta2+1;
	}
	int thirdhull=0;
	if(tb1)
		thirdhull = p1.numpoints-tb1;

	ConvexPolygon.numpoints = firsthull + secondhull + thirdhull;
								
	ConvexPolygon.points = (Point2D*)malloc(sizeof(Point2D)*ConvexPolygon.numpoints);
	memset(ConvexPolygon.points,0,sizeof(Point2D)*ConvexPolygon.numpoints);
	for(int i = 0; i < firsthull; i++)
		ConvexPolygon.points[i] = p1.points[i];
	for(int i = 0; i < secondhull; i++)
		ConvexPolygon.points[firsthull+i] = p2.points[(ta2+i)%p2.numpoints];
	for(int i = 0; i < thirdhull; i++)
		ConvexPolygon.points[firsthull+secondhull+i] = p1.points[tb1+i];
	
	return ConvexPolygon;
}

void MergePolygons(Polygon2D p1, Polygon2D p2){
	Polygon2D newpolygon = ConnectConvex(p1, p2);
	free(p1.points);
	free(p2.points);
	numpolygons--;
	polygons[numpolygons-1] = newpolygon;
}
