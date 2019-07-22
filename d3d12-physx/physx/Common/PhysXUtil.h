#pragma once

#include <ctype.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "PxPhysicsAPI.h"

//===========================================================
//===========================================================
// SnippetPrint
//===========================================================
//===========================================================

#ifndef PHYSX_SNIPPET_PRINT_H
#define PHYSX_SNIPPET_PRINT_H

#include "foundation/PxPreprocessor.h"

#if PX_XBOXONE
void OutputDebugPrint(const char*, ...);
#define printf OutputDebugPrint
#elif PX_SWITCH
#include "../SnippetCommon/Switch/SwitchSnippetPrint.h"
#endif

#endif // PHYSX_SNIPPET_PRINT_H

//===========================================================
//===========================================================
// PVD
//===========================================================
//===========================================================

#ifndef PHYSX_SNIPPET_PVD_H
#define PHYSX_SNIPPET_PVD_H

#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.

#endif //PHYSX_SNIPPET_PVD_H

//===========================================================
//===========================================================
// SnippetUtils
//===========================================================
//===========================================================

#include "../snippet-utils/SnippetUtils.h"

//===========================================================
//===========================================================
// PhysX¼¸ºÎÌå
//===========================================================
//===========================================================

enum PxGeometryEnum { PxSphere, PxBox, PxCapsule };

struct PxRigidDynamicDesc {

	// transform
	float px, py, pz;
	float qx, qy, qz, qw;

	// Material
	float materialStaticFriction;
	float materialDynamicFriction;
	float materialRestitution;

	// Geometry
	PxGeometryEnum pxGeometry;
	float gx;
	float gy;
	float gz;

	// density
	float density;
};