//
//  shape.h
//  Extension
//
//  Created by Dave Hayden on 10/7/15.
//  Copyright © 2015 Panic, Inc. All rights reserved.
//

#pragma once

#include "mini3d.h"
#include "../mathlib.h"

typedef struct
{
	int nPoints;
	const float3* points;
	int nFaces;
	const uint16_t* faces;
	const float* faceColorBias;
	float3 center; // used for z-sorting entire shapes at a time
} Shape3D;

void Shape3D_init(Shape3D* shape, int vtxCount, const float3* vb, int triCount, const uint16_t* ib, const float *triColorBias);
