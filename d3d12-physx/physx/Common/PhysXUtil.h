#pragma once

#include <ctype.h>

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
// 全局变量
//===========================================================
//===========================================================

extern PxDefaultAllocator		gAllocator;			// 默认的内存管理器
extern PxDefaultErrorCallback	gErrorCallback;		// 默认的错误管理器

extern PxFoundation* gFoundation;					// Px基础
extern PxPhysics* gPhysics;							// Px实例

extern PxDefaultCpuDispatcher* gDispatcher;	// cpu分配器
extern PxScene* gScene;						// 场景

extern PxPvd* gPvd;							// PVD