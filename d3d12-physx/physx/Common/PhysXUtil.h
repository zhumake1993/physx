#pragma once

#include <ctype.h>
#include <string>
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

using namespace physx;

//===========================================================
//===========================================================
// ȫ�ֱ���
//===========================================================
//===========================================================

extern PxDefaultAllocator		gAllocator;			// Ĭ�ϵ��ڴ������
extern PxDefaultErrorCallback	gErrorCallback;		// Ĭ�ϵĴ��������

extern PxFoundation* gFoundation;					// Px����
extern PxPhysics* gPhysics;							// Pxʵ��

extern PxDefaultCpuDispatcher* gDispatcher;	// cpu������
extern PxScene* gScene;						// ����

extern PxPvd* gPvd;							// PVD

//===========================================================
//===========================================================
// PhysX������
//===========================================================
//===========================================================

enum PxGeometryEnum { sphere, box, capsule, plane };

struct PxSphereGeometryDesc {
	float radius;
};

struct PxBoxGeometryDesc {
	float hx;
	float hy;
	float hz;
};

struct PxCapsuleGeometryDesc {
	float radius;
	float halfHeight;
};

struct PxPlaneGeometryDesc {
	float nx;
	float ny;
	float nz;
	float distance;
};

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
	void* PxGeometryDesc;

	// density
	float density;
};