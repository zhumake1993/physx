#pragma once

#include <string>

//===========================================================
//===========================================================
// PhysX������
//===========================================================
//===========================================================

struct PxFloat3 {
	PxFloat3() :x(0.0f), y(0.0f), z(0.0f) {}
	PxFloat3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
	float x;
	float y;
	float z;
};

struct PxFloat4 {
	PxFloat4() :x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	PxFloat4(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) {}
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

namespace CPT {
	struct PxCapsuleControllerDesc {
		PxFloat3 position;								// ��ʼλ��
		float contactOffset;							// Ƥ�����
		float stepOffset;								// ���¸߶�
		float slopeLimit;								// ���������¶�
		float radius;									// �뾶
		float height;									// �߶�
		PxFloat3 upDirection;							// �Ϸ���
	};
}


//===========================================================
//===========================================================
// �쳣�����
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