#include "PhysXUtil.h"

//===========================================================
//===========================================================
// 全局变量
//===========================================================
//===========================================================

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxPvd* gPvd = NULL;