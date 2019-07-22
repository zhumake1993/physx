#pragma once

#include "Common/d3dUtil.h"

class ShaderResource
{
public:
	ShaderResource(UINT width, UINT height, DXGI_FORMAT format);
		
	ShaderResource(const ShaderResource& rhs)=delete;
	ShaderResource& operator=(const ShaderResource& rhs)=delete;
	~ShaderResource()=default;

	ID3D12Resource* Resource();

private:
	void BuildResources();

private:
	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	Microsoft::WRL::ComPtr<ID3D12Resource> mShaderResource = nullptr;
};

 