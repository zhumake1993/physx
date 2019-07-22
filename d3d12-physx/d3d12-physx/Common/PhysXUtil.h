#pragma once

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