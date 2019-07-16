#include "Sphere.h"

Sphere::Sphere()
	:GameObject()
{
	mMatName = "mirror";

	mMeshName = "sphere";

	mRenderLayer = (int)RenderLayer::Opaque;
}

Sphere::~Sphere()
{
}

void Sphere::Update()
{
}
