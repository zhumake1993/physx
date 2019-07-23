#pragma once

#include "Common/D3DApp.h"

#include "Render/MainRender.h"
#include "Render/Wireframe.h"
#include "Render/DepthComplexityUseStencil.h"
#include "Render/DepthComplexityUseBlend.h"
#include "Render/DrawQuad.h"
#include "Render/RenderTarget.h"
#include "Render/ShaderResource.h"
#include "Render/CubeMap.h"
#include "Render/ShadowMap.h"
#include "Render/Ssao.h"

#include "Filter/BlurFilter.h"
#include "Filter/SobelFilter.h"
#include "Filter/InverseFilter.h"
#include "Filter/MultiplyFilter.h"

class D3D12App : public D3DApp
{
	

public:
	D3D12App(HINSTANCE hInstance);
	D3D12App(const D3D12App& rhs) = delete;
	D3D12App& operator=(const D3D12App& rhs) = delete;
	~D3D12App();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update()override;

	virtual void Draw()override;


	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	virtual void OnKeyDown(WPARAM vkCode)override;
	virtual void OnKeyUp(WPARAM vkCode)override;

	void OnKeyboardInput();

	void UpdateFrameResource();

	void BuildRenders();
	void BuildFilters();

	void Pick(int sx, int sy);

private:

	POINT mLastMousePos;

	DirectX::XMFLOAT3 mRotatedLightDirections[3];
	float mLightRotationAngle = 0.0f;
	DirectX::XMFLOAT3 mBaseLightDirections[3] = {
		DirectX::XMFLOAT3(0.57735f, -0.57735f, 0.57735f),
		DirectX::XMFLOAT3(-0.57735f, -0.57735f, 0.57735f),
		DirectX::XMFLOAT3(0.0f, -0.707f, -0.707f)
	};

	//
	// Render
	//

	std::unique_ptr<MainRender> mMainRender = nullptr;

	std::unique_ptr<DrawQuad> mDrawQuad = nullptr;
	std::unique_ptr<RenderTarget> mRenderTarget = nullptr;
	std::unique_ptr<ShaderResource> mShaderResourceTemp = nullptr;

	std::unique_ptr<Wireframe> mWireframe = nullptr;
	bool mIsWireframe = false;

	std::unique_ptr<DepthComplexityUseStencil> mDepthComplexityUseStencil = nullptr;
	bool mIsDepthComplexityUseStencil = false;

	std::unique_ptr<DepthComplexityUseBlend> mDepthComplexityUseBlend = nullptr;
	bool mIsDepthComplexityUseBlend = false;

	std::unique_ptr<CubeMap> mCubeMap = nullptr;
	std::unique_ptr<ShadowMap> mShadowMap = nullptr;
	std::unique_ptr<Ssao> mSsao = nullptr;

	//
	// Filter
	//

	std::unique_ptr<BlurFilter> mBlurFilter;
	bool mIsBlur = false;

	std::unique_ptr<SobelFilter> mSobelFilter;
	bool mIsSobel = false;

	std::unique_ptr<InverseFilter> mInverseFilter;
	std::unique_ptr<MultiplyFilter> mMultiplyFilter;
};

