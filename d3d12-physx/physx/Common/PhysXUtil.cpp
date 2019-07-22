#include "PhysXUtil.h"

using namespace physx;

//===========================================================
//===========================================================
// 全局变量
//===========================================================
//===========================================================

PxDefaultAllocator		gAllocator;				// 默认的内存管理器
PxDefaultErrorCallback	gErrorCallback;			// 默认的错误管理器

PxFoundation* gFoundation = NULL;				// Px基础
PxPhysics* gPhysics = NULL;						// Px实例

PxDefaultCpuDispatcher* gDispatcher = NULL;		// cpu分配器
PxScene* gScene = NULL;							// 场景

PxPvd* gPvd = NULL;								// PVD