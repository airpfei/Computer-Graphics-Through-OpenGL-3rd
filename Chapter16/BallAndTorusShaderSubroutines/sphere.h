#ifndef SPHERE_H
#define SPHERE_H

#include "vertex.h"

#define PI 3.14159265
#define SPH_RADIUS 2.0 // Sphere radius.
#define SPH_LONGS 10 // Number of longitudinal slices.
#define SPH_LATS 5 // Number of latitudinal slices.
#define SPH_COLORS 0.0, 0.0, 1.0, 1.0 // Sphere colors.

void fillSphVertexArray(Vertex sphVertices[(SPH_LONGS + 1) * (SPH_LATS + 1)]);
void fillSphIndices(unsigned int sphIndices[SPH_LATS][2*(SPH_LONGS+1)]);
void fillSphCounts(int sphCounts[SPH_LATS]);
void fillSphOffsets(void* sphOffsets[SPH_LATS]);

void fillSphere(Vertex sphVertices[(SPH_LONGS + 1) * (SPH_LATS + 1)], 
	         unsigned int sphIndices[SPH_LATS][2*(SPH_LONGS+1)],
			 int sphCounts[SPH_LATS],
			 void* sphOffsets[SPH_LATS]);

#endif