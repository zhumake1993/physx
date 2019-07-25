#include "d3dUtil.h"

using Microsoft::WRL::ComPtr;

//===========================================================
//===========================================================
// ȫ�ֱ���
//===========================================================
//===========================================================

std::wstring gMainWndCaption = L"d3d12app";							// ����
D3D_DRIVER_TYPE gd3dDriverType = D3D_DRIVER_TYPE_HARDWARE;			// Ӳ������
DXGI_FORMAT gBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;			// �󻺳��ʽ
DXGI_FORMAT gDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;	// ���ģ�建���ʽ
int gClientWidth = 1200;											// ��Ļ��
int gClientHeight = 900;											// ��Ļ��

ComPtr<ID3D12Device> gD3D12Device = nullptr;						// D3D12�豸
ComPtr<ID3D12GraphicsCommandList> gCommandList = nullptr;			// ָ���б�

bool g4xMsaaState = false;											// ���ز����Ƿ���
UINT g4xMsaaQuality = 0;											// ���ز�������

D3D12_VIEWPORT gScreenViewport;										// �ӿ�
D3D12_RECT gScissorRect;											// ���þ���

UINT gRtvDescriptorSize = 0;										// ��ȾĿ���������Ĵ�С
UINT gDsvDescriptorSize = 0;										// ���ģ���������Ĵ�С
UINT gCbvSrvUavDescriptorSize = 0;									// ������������������ɫ����Դ�������������ȡ�������Ĵ�С

extern const int gNumFrameResources = 3;							// ֡��Դ���������Ҫ�ڶ���ļ�֮�乲��const���󣬱����ڱ����Ķ���֮ǰ��extern�ؼ��֣�
int gCurrFrameResourceIndex = 0;									// ��ǰ֡��Դ����

#include "GameTimer.h"
GameTimer gTimer;													// ��ʱ��

#include "FrameResource.h"
std::unique_ptr<MainFrameResource> gMainFrameResource = std::make_unique<MainFrameResource>();;				// Main֡��Դ
std::unique_ptr<FrameResource<PassConstants>> gPassCB = std::make_unique<FrameResource<PassConstants>>();	// ��Ⱦ֡��Դ

#include "Camera.h"
std::unique_ptr<Camera> gCamera = std::make_unique<Camera>();

#include "Manager/SceneManager.h"
std::unique_ptr<SceneManager> gSceneManager = std::make_unique<SceneManager>();								// ����������

std::vector<D3D12_INPUT_ELEMENT_DESC> gInputLayout =														// ���벼��
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};
std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> gRootSignatures;								// ��ǩ��
std::unordered_map<std::string, ComPtr<ID3DBlob>> gShaders;													// ��ɫ��
std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> gPSOs;											// ��Ⱦ״̬����

#include "../physx/Main/PhysX.h"
PhysX gPhysX;																								// PhysX��������

//===========================================================
//===========================================================
// ��������
//===========================================================
//===========================================================

// ��һ��Transformת���ɾ�����ʽ
DirectX::XMMATRIX TransformToMatrix(Transform& transform)
{
	DirectX::XMVECTOR S = XMLoadFloat3(&transform.Scale);
	DirectX::XMVECTOR P = XMLoadFloat3(&transform.Translation);
	DirectX::XMVECTOR Q = XMLoadFloat4(&transform.Quaternion);

	DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	return DirectX::XMMatrixAffineTransformation(S, zero, Q, P);
}

//===========================================================
//===========================================================
// ������
//===========================================================
//===========================================================

ComPtr<ID3DBlob> d3dUtil::LoadBinary(const std::wstring& filename)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	ComPtr<ID3DBlob> blob;
	ThrowIfFailed(D3DCreateBlob(size, blob.GetAddressOf()));

	fin.read((char*)blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}

Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 byteSize,
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	ComPtr<ID3D12Resource> defaultBuffer;

	// ����ʵ�ʵ�Ĭ�ϻ�����Դ
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	// Ϊ�˽�CPU���ڴ����ݿ�����Ĭ�ϻ��壬��Ҫ�����м����ػ���
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	// ����Ҫ��������Դ
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// ���Ž����ݿ�����Ĭ�ϻ�����Դ
	// UpdateSubresources����CPU�ڴ濽�����м����ػ���
	// Then, using ID3D12CommandList::CopySubresourceRegion, the intermediate upload heap data will be copied to mBuffer.
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	// ע�⣺�������������ú�uploadBufferӦ���ּ���״̬����Ϊʵ��ִ�п���������ָ�δִ��
	// ��������֪�����������Ѿ�ִ����Ϻ�����ͷ�uploadBuffer

	return defaultBuffer;
}

ComPtr<ID3DBlob> d3dUtil::CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	return byteCode;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> d3dUtil::GetStaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC shadow(
		6, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,
		shadow };
}
