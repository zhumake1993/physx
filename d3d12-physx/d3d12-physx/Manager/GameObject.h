#pragma once

#include "Common/d3dUtil.h"
#include "Common/Camera.h"
#include "InstanceManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "InputManager.h"

using namespace DirectX;

//===========================================================
//===========================================================
// PhysX几何体
//===========================================================
//===========================================================

enum PxGeometryEnum { PxSphere, PxBox, PxCapsule, PxPlane };

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
	void* pxGeometryDesc;

	// density
	float density;
};

class GameObject
{
	friend class GameObjectManager;

public:
	GameObject();
	virtual ~GameObject();

protected:
	bool GetKeyDown(int key);
	bool GetKeyPress(int key);
	bool GetKeyUp(int key);

	void AddRigidBody();

	virtual void Update();

	XMFLOAT4X4 GetWorld();

public:
	std::string mGameObjectName = "GameObject";

	XMFLOAT3 mTranslation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 mRotationQuat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT3 mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	std::string mMatName;
	XMFLOAT4X4 mTexTransform = MathHelper::Identity4x4();

	std::string mMeshName;

	int mRenderLayer = -1;

	bool mReceiveShadow = true;

protected:

	bool mHasRigidBody = false;
	XMFLOAT3 mPxLocalPos;			// 相对位置
	XMFLOAT4 mPxLocalQuat;			// 相对四元数
	XMFLOAT3 mPxMaterial;
	PxGeometryEnum mPxGeometry;
	void* mPxGeometryDesc;
	float mDensity;
};