#include "PhysXUtil.h"

using namespace physx;

//===========================================================
//===========================================================
// ȫ�ֱ���
//===========================================================
//===========================================================

PxDefaultAllocator		gAllocator;				// Ĭ�ϵ��ڴ������
PxDefaultErrorCallback	gErrorCallback;			// Ĭ�ϵĴ��������

PxFoundation* gFoundation = NULL;				// Px����
PxPhysics* gPhysics = NULL;						// Pxʵ��

PxDefaultCpuDispatcher* gDispatcher = NULL;		// cpu������
PxScene* gScene = NULL;							// ����

PxPvd* gPvd = NULL;								// PVD