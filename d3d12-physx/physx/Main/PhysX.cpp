#include "PhysX.h"

#include <vector>

#include "Common/PhysXUtil.h"

#define MAX_NUM_ACTOR_SHAPES 128

// Ӳ���룬������
std::unordered_map<std::string, PxRigidDynamic*> gPxRigidDynamicMap;

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

void PhysX::CreatePxRigidStatic()
{
	// ��������
	PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// ������̬ƽ��
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *material);
	gScene->addActor(*groundPlane);
}

void PhysX::CreatePxRigidDynamic(std::string name, void* pdesc)
{
	auto desc = *static_cast<PxRigidDynamicDesc*>(pdesc);

	// ����transform
	auto transform = PxTransform(PxVec3(desc.px, desc.py, desc.pz), PxQuat(desc.qx, desc.qy, desc.qz, desc.qw));

	// ����actor
	PxRigidDynamic* actor = gPhysics->createRigidDynamic(transform);

	// ��������
	PxMaterial* material = gPhysics->createMaterial(desc.materialStaticFriction, desc.materialDynamicFriction, desc.materialRestitution);

	// ����shape
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

	// ���shape
	actor->attachShape(*shape);
	shape->release();

	// �����ܶ�
	PxRigidBodyExt::updateMassAndInertia(*actor, desc.density);

	// ���actor
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

	//��ȡtransform
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

	//		// ��ȡ��ӵ�actor��shape������
	//		const PxU32 nbShapes = actors[i]->getNbShapes();
	//		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);

	//		// ��ȡ��ӵ�actor��shape
	//		//actors[i]->getShapes(shapes, nbShapes);
	//		pg->getShapes(shapes, nbShapes);

	//		// ��ȡactor��sleep״̬
	//		const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

	//		for (PxU32 j = 0; j < nbShapes; j++)
	//		{
	//			// ��ȡtransform
	//			auto t = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);

	//			x = t.p.x;
	//			y = t.p.y;
	//			z = t.p.z;
	//			a = t.q.x;
	//			b = t.q.y;
	//			c = t.q.z;
	//			d = t.q.w;

	//			// ��ȡ������
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
