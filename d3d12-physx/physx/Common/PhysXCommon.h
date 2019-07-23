#pragma once

//===========================================================
//===========================================================
// PhysX¼¸ºÎÌå
//===========================================================
//===========================================================

enum PxGeometryEnum { PxSphere, PxBox, PxCapsule };

struct PxFloat3 {
	float x;
	float y;
	float z;
};

struct PxFloat4 {
	float x;
	float y;
	float z;
	float w;
};

struct PxRigidDynamicDesc {

	// transform
	PxFloat3 pos;
	PxFloat4 quat;

	// Material
	PxFloat3 material;

	// Geometry
	PxGeometryEnum pxGeometry;
	PxFloat3 scale;

	// density
	float density;
};