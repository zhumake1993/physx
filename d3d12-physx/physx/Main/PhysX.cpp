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

// 硬编码，测试用
std::unordered_map<std::string, PxRigidDynamic*> gPxRigidDynamicMap;

PhysX::PhysX()
{
}

PhysX::~PhysX()
{
}

void PhysX::InitPhysics()
{
	// 创建Px基础
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	// 链接Pvd
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	// 创建Px实例
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
}

void PhysX::CreateScene()
{
	// 创建场景
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	// 设置传输到Pvd的数据
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);	// 开启约束数据传输到Pvd
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);		// 开启链接数据传输到Pvd
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);	// 开启场景查询数据传输到Pvd
	}
}

void PhysX::CreatePxRigidStatic()
{
	// 创建材质
	PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// 创建静态平面
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *material);
	gScene->addActor(*groundPlane);
}

void PhysX::CreatePxRigidDynamic(std::string name, void* pdesc)
{
	auto desc = *static_cast<PxRigidDynamicDesc*>(pdesc);

	// 创建transform
	auto transform = PxTransform(PxVec3(desc.pos.x, desc.pos.y, desc.pos.z), PxQuat(desc.quat.x, desc.quat.y, desc.quat.z, desc.quat.w));

	// 创建actor
	PxRigidDynamic* actor = gPhysics->createRigidDynamic(transform);

	// 创建材质
	PxMaterial* material = gPhysics->createMaterial(desc.material.x, desc.material.y, desc.material.z);

	// 创建shape
	PxShape* shape = nullptr;
	switch (desc.pxGeometry) {
		case PxSphere: {
			shape = gPhysics->createShape(PxSphereGeometry(desc.scale.x), *material); break;
		}
		case PxBox: {
			shape = gPhysics->createShape(PxBoxGeometry(desc.scale.x, desc.scale.y, desc.scale.z), *material); break;
		}
		case PxCapsule: {
			shape = gPhysics->createShape(PxCapsuleGeometry(desc.scale.x, desc.scale.y), *material); break;
		}
		default: {
			assert(shape);
		}
	}

	// 添加shape
	actor->attachShape(*shape);
	shape->release();

	// 设置密度
	PxRigidBodyExt::updateMassAndInertia(*actor, desc.density);

	// 添加actor
	gScene->addActor(*actor);

	gPxRigidDynamicMap[name] = actor;
}

void PhysX::Update(float delta)
{
	/*gScene->simulate(delta);
	gScene->fetchResults(true);*/

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

	//获取transform
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
