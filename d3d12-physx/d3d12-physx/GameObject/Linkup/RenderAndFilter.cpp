#include "RenderAndFilter.h"

using namespace DirectX;

RenderAndFilter::RenderAndFilter(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
}

RenderAndFilter::~RenderAndFilter()
{
}

void RenderAndFilter::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	if (GetKeyDown('1')) {
		SetIsWireframe(!GetIsWireframe());
		SetIsDepthComplexityUseStencil(false);
		SetIsDepthComplexityUseBlend(false);
	}

	if (GetKeyDown('2')) {
		SetIsDepthComplexityUseStencil(!GetIsDepthComplexityUseStencil());
		SetIsWireframe(false);
		SetIsDepthComplexityUseBlend(false);
	}

	if (GetKeyDown('3')) {
		SetIsDepthComplexityUseBlend(!GetIsDepthComplexityUseBlend());
		SetIsWireframe(false);
		SetIsDepthComplexityUseStencil(false);
	}

	if (GetKeyDown('4')) {
		SetIsBlur(!GetIsBlur());
	}

	if (GetKeyDown('5')) {
		SetIsSobel(!GetIsSobel());
	}

	if (GetKeyDown('6')) {
		GetMainCamera()->SetFrustumCulling(GetMainCamera()->GetFrustumCulling());
	}

	if (GetKeyDown('7')) {
		SetIsDrawRigidbody(!GetIsDrawRigidbody());
	}

	if (GetKeyDown('8')) {
		SetIsShadowMap(!GetIsShadowMap());
	}

	if (GetKeyDown('9')) {
		SetIsSsao(!GetIsSsao());
	}
}
