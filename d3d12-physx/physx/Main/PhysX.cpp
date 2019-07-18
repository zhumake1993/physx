#include "PhysX.h"

#include <vector>

#include "Common/PhysXUtil.h"

#define MAX_NUM_ACTOR_SHAPES 128

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
	auto transform = PxTransform(PxVec3(desc.px, desc.py, desc.pz), PxQuat(desc.qx, desc.qy, desc.qz, desc.qw));

	// 创建actor
	PxRigidDynamic* actor = gPhysics->createRigidDynamic(transform);

	// 创建材质
	PxMaterial* material = gPhysics->createMaterial(desc.materialStaticFriction, desc.materialDynamicFriction, desc.materialRestitution);

	// 创建shape
	PxShape* shape = nullptr;
	switch (desc.pxGeometry) {
		case sphere: {
			auto p = static_cast<PxSphereGeometryDesc*>(desc.PxGeometryDesc);
			shape = gPhysics->createShape(PxSphereGeometry(p->radius), *material); break;
		}
		case box: {
			auto p = static_cast<PxBoxGeometryDesc*>(desc.PxGeometryDesc);
			shape = gPhysics->createShape(PxBoxGeometry(p->hx, p->hy, p->hz), *material); break;
		}
		case capsule: {
			auto p = static_cast<PxCapsuleGeometryDesc*>(desc.PxGeometryDesc);
			shape = gPhysics->createShape(PxCapsuleGeometry(p->radius, p->halfHeight), *material); break;
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
	gScene->simulate(delta);
	gScene->fetchResults(true);
}

void PhysX::Get(float& x, float& y, float& z, float& a, float& b, float& c, float& d)
{
	auto pg = gPxRigidDynamicMap["boxPx"];
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	pg->getShapes(shapes, 1);

	//获取transform
	auto t = PxShapeExt::getGlobalPose(*shapes[0], *pg);

	x = t.p.x;
	y = t.p.y;
	z = t.p.z;
	a = t.q.x;
	b = t.q.y;
	c = t.q.z;
	d = t.q.w;

	//PxScene* scene;
	//PxGetPhysics().getScenes(&scene, 1);
	//PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	//if (nbActors)
	//{
	//	std::vector<PxRigidActor*> actors(nbActors);
	//	scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor * *>(&actors[0]), nbActors);
	//	//Snippets::renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);

	//	PxU32 numActors = static_cast<PxU32>(actors.size());
	//	
	//	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

	//	for (PxU32 i = 0; i < numActors; i++)
	//	{
	//		auto pg = static_cast<PxRigidActor*>(gg);

	//		// 获取添加到actor的shape的数量
	//		const PxU32 nbShapes = actors[i]->getNbShapes();
	//		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);

	//		// 获取添加到actor的shape
	//		//actors[i]->getShapes(shapes, nbShapes);
	//		pg->getShapes(shapes, nbShapes);

	//		// 获取actor的sleep状态
	//		const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

	//		for (PxU32 j = 0; j < nbShapes; j++)
	//		{
	//			// 获取transform
	//			auto t = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);

	//			x = t.p.x;
	//			y = t.p.y;
	//			z = t.p.z;
	//			a = t.q.x;
	//			b = t.q.y;
	//			c = t.q.z;
	//			d = t.q.w;

	//			// 获取几何体
	//			const PxGeometryHolder h = shapes[j]->getGeometry();

	//			if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE) {
	//				//
	//			}

	//			if (sleeping)
	//			{
	//				//
	//			}
	//			else
	//			{
	//				//
	//			}
	//		}
	//	}
	//}
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
