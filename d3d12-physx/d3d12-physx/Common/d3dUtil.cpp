#include "d3dUtil.h"

using Microsoft::WRL::ComPtr;

//===========================================================
//===========================================================
// 全局变量
//===========================================================
//===========================================================

std::wstring gMainWndCaption = L"d3d12app";							// 标题
D3D_DRIVER_TYPE gd3dDriverType = D3D_DRIVER_TYPE_HARDWARE;			// 硬件类型
DXGI_FORMAT gBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;			// 后缓冲格式
DXGI_FORMAT gDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 深度模板缓冲格式
int gClientWidth = 1200;											// 屏幕宽
int gClientHeight = 900;											// 屏幕高

ComPtr<ID3D12Device> gD3D12Device = nullptr;						// D3D12设备
ComPtr<ID3D12GraphicsCommandList> gCommandList = nullptr;			// 指令列表

bool g4xMsaaState = false;											// 多重采样是否开启
UINT g4xMsaaQuality = 0;											// 多重采样质量

D3D12_VIEWPORT gScreenViewport;										// 视口
D3D12_RECT gScissorRect;											// 剪裁矩形

UINT gRtvDescriptorSize = 0;										// 渲染目标描述符的大小
UINT gDsvDescriptorSize = 0;										// 深度模板描述符的大小
UINT gCbvSrvUavDescriptorSize = 0;									// 常量缓冲描述符，着色器资源描述符，无序存取描述符的大小

extern const int gNumFrameResources = 3;							// 帧资源数量（如果要在多个文件之间共享const对象，必须在变量的定义之前加extern关键字）
int gCurrFrameResourceIndex = 0;									// 当前帧资源索引

#include "GameTimer.h"
GameTimer gTimer;													// 计时器

#include "FrameResource.h"
std::unique_ptr<MainFrameResource> gMainFrameResource = std::make_unique<MainFrameResource>();;				// Main帧资源
std::unique_ptr<FrameResource<PassConstants>> gPassCB = std::make_unique<FrameResource<PassConstants>>();	// 渲染帧资源

#include "Camera.h"
std::unique_ptr<Camera> gCamera = std::make_unique<Camera>();

#include "Manager/SceneManager.h"
std::unique_ptr<SceneManager> gSceneManager = std::make_unique<SceneManager>();								// 场景管理器

std::vector<D3D12_INPUT_ELEMENT_DESC> gInputLayout =														// 输入布局
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};
std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> gRootSignatures;								// 根签名
std::unordered_map<std::string, ComPtr<ID3DBlob>> gShaders;													// 着色器
std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> gPSOs;											// 渲染状态对象

#include "../physx/Main/PhysX.h"
PhysX gPhysX;																								// PhysX物理引擎

//===========================================================
//===========================================================
// 辅助函数
//===========================================================
//===========================================================

// 将一个Transform转换成矩阵形式
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
// 辅助类
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

	// 创建实际的默认缓冲资源
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	// 为了将CPU的内存数据拷贝到默认缓冲，需要创建中间上载缓冲
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	// 描述要拷贝的资源
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// 安排将数据拷贝至默认缓冲资源
	// UpdateSubresources负责将CPU内存拷贝至中间上载缓冲
	// Then, using ID3D12CommandList::CopySubresourceRegion, the intermediate upload heap data will be copied to mBuffer.
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	// 注意：在上述函数调用后uploadBuffer应保持激活状态，因为实际执行拷贝操作的指令还未执行
	// 调用者在知道拷贝操作已经执行完毕后才能释放uploadBuffer

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
