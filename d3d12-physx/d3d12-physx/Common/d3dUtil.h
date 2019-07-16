#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <array>
#include <fstream>
#include <sstream>
#include <comdef.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include "d3dx12.h"
#include "MathHelper.h"
#include "GameTimer.h"
#include "DDSTextureLoader.h"
#include "../physx/Main/PhysX.h"

using Microsoft::WRL::ComPtr;

//===========================================================
//===========================================================
// 全局变量
//===========================================================
//===========================================================

extern std::wstring gMainWndCaption;					// 标题
extern D3D_DRIVER_TYPE gd3dDriverType;					// 硬件类型
extern DXGI_FORMAT gBackBufferFormat;					// 后缓冲格式
extern DXGI_FORMAT gDepthStencilFormat;					// 深度模板缓冲格式
extern int gClientWidth;								// 屏幕宽
extern int gClientHeight;								// 屏幕高

extern GameTimer gTimer;								// 计时器

extern ComPtr<ID3D12Device> gD3D12Device;				// D3D12设备
extern ComPtr<ID3D12GraphicsCommandList> gCommandList;	// 指令列表

extern bool g4xMsaaState;								// 多重采样是否开启
extern UINT g4xMsaaQuality;								// 多重采样质量

extern D3D12_VIEWPORT gScreenViewport;					// 视口
extern D3D12_RECT gScissorRect;							// 剪裁矩形

extern UINT gRtvDescriptorSize;							// 渲染目标描述符的大小
extern UINT gDsvDescriptorSize;							// 深度模板描述符的大小
extern UINT gCbvSrvUavDescriptorSize;					// 常量缓冲描述符，着色器资源描述符，无序存取描述符的大小

const int gNumFrameResources = 3;						// 帧资源数量
extern int gCurrFrameResourceIndex;						// 当前帧资源索引

extern PhysX gPhysX;									// PhysX物理引擎

//===========================================================
//===========================================================
// 顶点、输入布局、根签名、着色器、渲染状态对象
//===========================================================
//===========================================================

// 顶点
struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
	DirectX::XMFLOAT3 TangentU;
};

// 输入布局
extern std::vector<D3D12_INPUT_ELEMENT_DESC> gInputLayout;

// 根签名
extern std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> gRootSignatures;

// 着色器
extern std::unordered_map<std::string, ComPtr<ID3DBlob>> gShaders;

// 渲染状态对象
extern std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> gPSOs;

//===========================================================
//===========================================================
// 光
//===========================================================
//===========================================================

#define MaxLights 16
struct Light
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // 点光/聚光
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// 平行光/聚光
	float FalloffEnd = 10.0f;                           // 点光/聚光
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // 点光/聚光
	float SpotPower = 64.0f;                            // 聚光
};

//===========================================================
//===========================================================
// 渲染层
//===========================================================
//===========================================================

enum class RenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	Sky,
	OpaqueDynamicReflectors,
	UI,

	Count
};

//===========================================================
//===========================================================
// 辅助类
//===========================================================
//===========================================================

class d3dUtil
{
public:

	// 计算常量缓冲的字节大小
	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// 量常缓冲的大小必须是硬件分配大小（通常256字节）的最小整数倍
		// 因此需要规整
		// 样例：byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}

	static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);

	static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
};

//===========================================================
//===========================================================
// 异常与调试
//===========================================================
//===========================================================

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	std::wstring ToString()const {
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();
		return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

inline std::wstring StringToWString(const std::string& str)
{
	int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wide = new wchar_t[num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
}

inline std::string WStringToString(const std::wstring& wstr)
{
	std::string str;
	int nLen = (int)wstr.length();
	str.resize(nLen, ' ');
	int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);
	if (nResult == 0) {
		return "";
	}
	return str;
}

inline void OutputDebug(int i)
{
	std::wstring text = L"OutputDebug: ";
	text += std::to_wstring(i);
	text += L"\n";
	OutputDebugString(text.c_str());
}

inline void OutputDebug(float f)
{
	std::wstring text = L"OutputDebug: ";
	text += std::to_wstring(f);
	text += L"\n";
	OutputDebugString(text.c_str());
}

inline void OutputDebug(std::string s)
{
	std::wstring text = L"OutputDebug: ";
	text += StringToWString(s);
	text += L"\n";
	OutputDebugString(text.c_str());
}

inline void OutputMessageBox(int i)
{
	MessageBox(nullptr, std::to_wstring(i).c_str(), L"Debug", MB_OK);
}

inline void OutputMessageBox(std::string s)
{
	MessageBox(nullptr, StringToWString(s).c_str(), L"Debug", MB_OK);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif