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
#include "DDSTextureLoader.h"
#include "../physx/Common/PhysXCommon.h"

//===========================================================
//===========================================================
// 结构体
//===========================================================
//===========================================================

struct Transform
{
	Transform(const DirectX::XMFLOAT3& t = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DirectX::XMFLOAT4& q = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		const DirectX::XMFLOAT3& s = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)) :
		Translation(t),
		Quaternion(q),
		Scale(s) {}
	DirectX::XMFLOAT3 Translation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 Quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 Scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
	DirectX::XMFLOAT3 TangentU;
};

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

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProjTex = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ShadowTransform = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT4 FogColor = { 0.7f, 0.7f, 0.7f, 1.0f };
	float gFogStart = 5.0f;
	float gFogRange = 150.0f;
	DirectX::XMFLOAT2 cbPerObjectPad2;

	// 索引 [0, NUM_DIR_LIGHTS) 是平行光
	// 索引 [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) 是点光
	// 索引 [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS) 是聚光
	Light Lights[MaxLights];
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
	Wireframe,

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

class MyException
{
public:
	MyException() = default;
	MyException(const std::string& err, const std::wstring& filename, int lineNumber) :
		Err(err) ,
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	std::wstring ToString()const { 
		return StringToWString(Err) + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber);
	}

	std::string Err;
	std::wstring Filename;
	int LineNumber = -1;
};

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

#ifndef ThrowMyEx
#define ThrowMyEx(x)										          \
{                                                                     \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    throw MyException(x, wfn, __LINE__);							  \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif