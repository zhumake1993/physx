#include "RenderTarget.h"
 
RenderTarget::RenderTarget(UINT width, UINT height, DXGI_FORMAT format)
{
	mWidth = width;
	mHeight = height;
	mFormat = format;

	BuildResources();
	BuildDescriptors();
}

ID3D12Resource* RenderTarget::Resource()
{
	return mRenderTargetTex.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::Rtv()
{
	return mhCpuRtv;
}

void RenderTarget::BuildResources()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&CD3DX12_CLEAR_VALUE(mFormat, DirectX::Colors::Black), //LightSteelBlue
		IID_PPV_ARGS(&mRenderTargetTex)));
}

void RenderTarget::BuildDescriptors()
{
	// 创建RTV堆
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = 1;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptorRtv(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

	// 保留描述符的引用
	mhCpuRtv = hCpuDescriptorRtv;

	gD3D12Device->CreateRenderTargetView(mRenderTargetTex.Get(), nullptr, mhCpuRtv);
}