#ifndef INTERSECTIONDETECTIONROUTINES_H
#define INTERSECTIONDETECTIONROUTINES_H



int checkQuadrilateralsIntersection(float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4,
	float x5, float y5, float x6, float y6,
	float x7, float y7, float x8, float y8);

int checkDiscRectangleIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float r);

#endif