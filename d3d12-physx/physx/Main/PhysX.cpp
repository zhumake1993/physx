#include "PhysX.h"

#include "Common/PhysXUtil.h"

using namespace physx;

#define MAX_NUM_ACTOR_SHAPES 128

extern PxDefaultAllocator		gAllocator;
extern PxDefaultErrorCallback	gErrorCallback;

extern PxFoundation* gFoundation;
extern PxPhysics* gPhysics;

extern PxDefaultCpuDispatcher* gDispatcher;
extern PxScene* gScene;

extern PxPvd* gPvd;

// Ӳ���룬������
std::unordered_map<std::string, PxRigidDynamic*> gPxRigidDynamicMap;
std::unordered_map<std::string, PxRigidStatic*> gPxRigidStaticMap;

PhysX::PhysX()
{
}

PhysX::~PhysX()
{
}

void PhysX::InitPhysics()
{
	// ����Px����
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	// ����Pvd
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	// ����Pxʵ��
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
}

void PhysX::CreateScene()
{
	// ��������
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	// ���ô��䵽Pvd������
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);	// ����Լ�����ݴ��䵽Pvd
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);		// �����������ݴ��䵽Pvd
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);	// ����������ѯ���ݴ��䵽Pvd
	}
}

void PhysX::CleanupScene()
{
	gPxRigidDynamicMap.clear();
	gPxRigidStaticMap.clear();
	PX_RELEASE(gScene);
}

void PhysX::CreatePxRigidStatic(std::string name, void* pdesc)
{
	if (HasPxRigidStatic(name)) {
		ThrowPxEx("RigidStatic already exists!");
	}

	auto desc = *static_cast<PxRigidStaticDesc*>(pdesc);

	// ����transform
	auto transform = PxTransform(PxVec3(desc.pos.x, desc.pos.y, desc.pos.z), PxQuat(desc.quat.x, desc.quat.y, desc.quat.z, desc.quat.w));

	// ����actor
	PxRigidStatic* actor = gPhysics->createRigidStatic(transform);

	// ��������
	PxMaterial* material = gPhysics->createMaterial(desc.material.x, desc.material.y, desc.material.z);

	// ����shape
	PxShape* shape = nullptr;
	switch (desc.pxGeometry) {
		case PxSphereEnum: {
			shape = gPhysics->createShape(PxSphereGeometry(desc.scale.x), *material); 
			break;
		}
		case PxBoxEnum: {
			shape = gPhysics->createShape(PxBoxGeometry(desc.scale.x, desc.scale.y, desc.scale.z), *material); 
			break;
		}
		case PxCapsuleEnum: {
			shape = gPhysics->createShape(PxCapsuleGeometry(desc.scale.x, desc.scale.y), *material); 
			break;
		}
		case PxPlaneEnum: {
			ThrowPxEx("Plane not implemented!");
			break;
		}
		default: {
			ThrowPxEx("Wrong Geometry!");
		}
	}

	// ���shape
	actor->attachShape(*shape);
	shape->release();

	// ���actor
	gScene->addActor(*actor);

	gPxRigidStaticMap[name] = actor;
}

void PhysX::CreatePxRigidDynamic(std::string name, void* pdesc)
{
	if (HasPxRigidDynamic(name)) {
		ThrowPxEx("RigidDynamic already exists!");
	}

	auto desc = *static_cast<PxRigidDynamicDesc*>(pdesc);

	// ����transform
	auto transform = PxTransform(PxVec3(desc.pos.x, desc.pos.y, desc.pos.z), PxQuat(desc.quat.x, desc.quat.y, desc.quat.z, desc.quat.w));

	// ����actor
	PxRigidDynamic* actor = gPhysics->createRigidDynamic(transform);

	// ��������
	PxMaterial* material = gPhysics->createMaterial(desc.material.x, desc.material.y, desc.material.z);

	// ����shape
	PxShape* shape = nullptr;
	switch (desc.pxGeometry) {
		case PxSphereEnum: {
			shape = gPhysics->createShape(PxSphereGeometry(desc.scale.x), *material); break;
		}
		case PxBoxEnum: {
			shape = gPhysics->createShape(PxBoxGeometry(desc.scale.x, desc.scale.y, desc.scale.z), *material); break;
		}
		case PxCapsuleEnum: {
			shape = gPhysics->createShape(PxCapsuleGeometry(desc.scale.x, desc.scale.y), *material); break;
		}
		default: {
			ThrowPxEx("Wrong Geometry!");
		}
	}

	// ���shape
	actor->attachShape(*shape);
	shape->release();

	// �����ܶ�
	PxRigidBodyExt::updateMassAndInertia(*actor, desc.density);

	// ���actor
	gScene->addActor(*actor);

	gPxRigidDynamicMap[name] = actor;
}

void PhysX::DeletePxRigid(std::string name)
{
	if (HasPxRigidDynamic(name)) {
		gPxRigidDynamicMap[name]->release();
	}
	else if (HasPxRigidStatic(name)) {
		gPxRigidStaticMap[name]->release();
	}
	else {
		ThrowPxEx("Rigid does not exist!");
	}
}

void PhysX::Update(float delta)
{
	mAccumulator += delta;
	if (mAccumulator < mStepSize)
		return;

	mAccumulator -= mStepSize;

	gScene->simulate(mStepSize);
	gScene->fetchResults(true);
}

void PhysX::GetPxRigidDynamicTransform(std::string name, PxFloat3& pos, PxFloat4& quat)
{
	auto pg = gPxRigidDynamicMap[name];
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	pg->getShapes(shapes, 1);

	//��ȡtransform
	auto t = PxShapeExt::getGlobalPose(*shapes[0], *pg);

	pos.x = t.p.x;
	pos.y = t.p.y;
	pos.z = t.p.z;
	quat.x = t.q.x;
	quat.y = t.q.y;
	quat.z = t.q.z;
	quat.w = t.q.w;
}

void PhysX::CleanupPhysics()
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	printf("SnippetHelloWorld done.\n");
}

bool PhysX::HasPxRigidStatic(const std::string& name)
{
	return gPxRigidStaticMap.find(name) != gPxRigidStaticMap.end();
}

bool PhysX::HasPxRigidDynamic(const std::string& name)
{
	return gPxRigidDynamicMap.find(name) != gPxRigidDynamicMap.end();
}
