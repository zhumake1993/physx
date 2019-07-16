#include "PhysX.h"

#include <vector>

#include "Common/PhysXUtil.h"

#define MAX_NUM_ACTOR_SHAPES 128

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

void PhysX::CreateObject()
{
	// ��������
	PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// ������̬ƽ��
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *material);
	gScene->addActor(*groundPlane);

	// ������̬������
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *material);
	PxRigidDynamic* body = gPhysics->createRigidDynamic(PxTransform(PxVec3(0.0f, 10.0f, 0.0f)));
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
	shape->release();
}

void PhysX::Update(float delta)
{
	gScene->simulate(delta);
	gScene->fetchResults(true);
}

void PhysX::Get(float& x, float& y, float& z, float& a, float& b, float& c, float& d)
{
	PxScene* scene;
	PxGetPhysics().getScenes(&scene, 1);
	PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor * *>(&actors[0]), nbActors);
		//Snippets::renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);

		PxU32 numActors = static_cast<PxU32>(actors.size());
		
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

		for (PxU32 i = 0; i < numActors; i++)
		{
			// ��ȡ��ӵ�actor��shape������
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);

			// ��ȡ��ӵ�actor��shape
			actors[i]->getShapes(shapes, nbShapes);

			// ��ȡactor��sleep״̬
			const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

			for (PxU32 j = 0; j < nbShapes; j++)
			{
				// ��ȡtransform
				auto t = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);

				x = t.p.x;
				y = t.p.y;
				z = t.p.z;
				a = t.q.x;
				b = t.q.y;
				c = t.q.z;
				d = t.q.w;

				// ��ȡ������
				const PxGeometryHolder h = shapes[j]->getGeometry();

				if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE) {
					//
				}

				if (sleeping)
				{
					//
				}
				else
				{
					//
				}
			}
		}
	}
}

void PhysX::cleanupPhysics()
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
