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
// ȫ�ֱ���
//===========================================================
//===========================================================

extern std::wstring gMainWndCaption;					// ����
extern D3D_DRIVER_TYPE gd3dDriverType;					// Ӳ������
extern DXGI_FORMAT gBackBufferFormat;					// �󻺳��ʽ
extern DXGI_FORMAT gDepthStencilFormat;					// ���ģ�建���ʽ
extern int gClientWidth;								// ��Ļ��
extern int gClientHeight;								// ��Ļ��

extern GameTimer gTimer;								// ��ʱ��

extern ComPtr<ID3D12Device> gD3D12Device;				// D3D12�豸
extern ComPtr<ID3D12GraphicsCommandList> gCommandList;	// ָ���б�

extern bool g4xMsaaState;								// ���ز����Ƿ���
extern UINT g4xMsaaQuality;								// ���ز�������

extern D3D12_VIEWPORT gScreenViewport;					// �ӿ�
extern D3D12_RECT gScissorRect;							// ���þ���

extern UINT gRtvDescriptorSize;							// ��ȾĿ���������Ĵ�С
extern UINT gDsvDescriptorSize;							// ���ģ���������Ĵ�С
extern UINT gCbvSrvUavDescriptorSize;					// ������������������ɫ����Դ�������������ȡ�������Ĵ�С

const int gNumFrameResources = 3;						// ֡��Դ����
extern int gCurrFrameResourceIndex;						// ��ǰ֡��Դ����

extern PhysX gPhysX;									// PhysX��������

//===========================================================
//===========================================================
// ���㡢���벼�֡���ǩ������ɫ������Ⱦ״̬����
//===========================================================
//===========================================================

// ����
struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
	DirectX::XMFLOAT3 TangentU;
};

// ���벼��
extern std::vector<D3D12_INPUT_ELEMENT_DESC> gInputLayout;

// ��ǩ��
extern std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> gRootSignatures;

// ��ɫ��
extern std::unordered_map<std::string, ComPtr<ID3DBlob>> gShaders;

// ��Ⱦ״̬����
extern std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> gPSOs;

//===========================================================
//===========================================================
// ��
//===========================================================
//===========================================================

#define MaxLights 16
struct Light
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // ���/�۹�
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// ƽ�й�/�۹�
	float FalloffEnd = 10.0f;                           // ���/�۹�
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // ���/�۹�
	float SpotPower = 64.0f;                            // �۹�
};

//===========================================================
//===========================================================
// ��Ⱦ��
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
// ������
//===========================================================
//===========================================================

class d3dUtil
{
public:

	// ���㳣��������ֽڴ�С
	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// ��������Ĵ�С������Ӳ�������С��ͨ��256�ֽڣ�����С������
		// �����Ҫ����
		// ������byteSize = 300.
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
// �쳣�����
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