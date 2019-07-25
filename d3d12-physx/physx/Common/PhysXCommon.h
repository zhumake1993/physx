#pragma once

#include <string>

//===========================================================
//===========================================================
// PhysX几何体
//===========================================================
//===========================================================

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

enum PxGeometryEnum { PxSphereEnum, PxBoxEnum, PxCapsuleEnum, PxPlaneEnum };

struct PxRigidDynamicDesc {

	// transform
	PxFloat3 pos;
	PxFloat4 quat;

	// Material
	PxFloat3 material;

	// Geometry
	PxGeometryEnum pxGeometry;
	PxFloat4 scale;

	// density
	float density;
};

struct PxRigidStaticDesc {

	// transform
	PxFloat3 pos;
	PxFloat4 quat;

	// Material
	PxFloat3 material;

	// Geometry
	PxGeometryEnum pxGeometry;
	PxFloat4 scale;
};

//===========================================================
//===========================================================
// 异常与调试
//===========================================================
//===========================================================

class MyPxException
{
public:
	MyPxException() = default;
	MyPxException(const std::string& err, const std::string& filename, int lineNumber) :
		Err(err),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	std::string ToString()const {
		return Err + " failed in " + Filename + "; line " + std::to_string(LineNumber);
	}

	std::string Err;
	std::string Filename;
	int LineNumber = -1;
};

#ifndef ThrowPxEx
#define ThrowPxEx(x)										          \
{                                                                     \
    throw MyPxException(x, __FILE__, __LINE__);					      \
}
#endif