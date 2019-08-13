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

std::string PhysX::CreatePxRigidStatic(void* pdesc)
{
	auto name = MathHelper::RandStr();
	while (HasPxRigidStatic(name)) {
		name = MathHelper::RandStr();
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

	return name;
}

std::string PhysX::CreatePxRigidDynamic(void* pdesc)
{
	auto name = MathHelper::RandStr();
	while (HasPxRigidDynamic(name)) {
		name = MathHelper::RandStr();
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

	return name;
}

void PhysX::AddForce(std::string name, PxFloat3 v)
{
	if (!HasPxRigidDynamic(name)) {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}

	PxVec3 force = PxVec3(v.x, v.y, v.z);
	gPxRigidDynamicMap[name]->addForce(force);
}

void PhysX::SetRigidDynamicLockFlag(std::string name, int axis, bool st)
{
	if (!HasPxRigidDynamic(name)) {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}

	switch (axis) {
		case 0:gPxRigidDynamicMap[name]->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, st); break;
		case 1:gPxRigidDynamicMap[name]->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, st); break;
		case 2:gPxRigidDynamicMap[name]->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, st); break;
		case 3:gPxRigidDynamicMap[name]->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, st); break;
		case 4:gPxRigidDynamicMap[name]->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, st); break;
		case 5:gPxRigidDynamicMap[name]->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, st); break;
		default:ThrowPxEx("Wrong axis!");
	}
}

void PhysX::SetAngularDamping(std::string name, float ad)
{
	if (!HasPxRigidDynamic(name)) {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}

	gPxRigidDynamicMap[name]->setAngularDamping(ad);
}

void PhysX::SetLinearVelocity(std::string name, PxFloat3 v)
{
	if (!HasPxRigidDynamic(name)) {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}

	PxVec3 velocity = PxVec3(v.x, v.y, v.z);
	gPxRigidDynamicMap[name]->setLinearVelocity(velocity);
}

void PhysX::SetKinematicFlag(std::string name, bool st)
{
	if (!HasPxRigidDynamic(name)) {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}

	gPxRigidDynamicMap[name]->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, st);
}

void PhysX::SetKinematicTarget(std::string name, PxFloat3& pos, PxFloat4& quat)
{
	if (!HasPxRigidDynamic(name)) {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}

	auto target = PxTransform(PxVec3(pos.x, pos.y, pos.z), PxQuat(quat.x, quat.y, quat.z, quat.w));
	gPxRigidDynamicMap[name]->setKinematicTarget(target);
}

void PhysX::DeletePxRigidDynamic(std::string name)
{
	if (HasPxRigidDynamic(name)) {
		gPxRigidDynamicMap[name]->release();
	}
	else {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}
}

void PhysX::DeletePxRigidStatic(std::string name)
{
	if (HasPxRigidStatic(name)) {
		gPxRigidStaticMap[name]->release();
	}
	else {
		ThrowPxEx("PxRigidStatic does not exist!");
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
	if (!HasPxRigidDynamic(name)) {
		ThrowPxEx("PxRigidDynamic does not exist!");
	}

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
