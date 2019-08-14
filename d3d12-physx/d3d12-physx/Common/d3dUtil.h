#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <comdef.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include "d3dx12.h"
#include "Log.h"
#include "MathHelper.h"
#include "MyException.h"
#include "DDSTextureLoader.h"
#include "../physx/Common/PhysXCommon.h"

//===========================================================
//===========================================================
// �ṹ��
//===========================================================
//===========================================================

struct Setting {
	std::wstring MainWndCaption = L"d3d12app";							// ����
	D3D_DRIVER_TYPE D3dDriverType = D3D_DRIVER_TYPE_HARDWARE;			// Ӳ������
	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;			// �󻺳��ʽ
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;		// ���ģ�建���ʽ
	int ClientWidth = 1200;												// ��Ļ��
	int ClientHeight = 900;												// ��Ļ��

	bool X4MsaaState = false;											// ���ز����Ƿ���
	UINT X4MsaaQuality = 0;												// ���ز�������

	D3D12_VIEWPORT ScreenViewport;										// �ӿ�
	D3D12_RECT ScissorRect;												// ���þ���

	UINT RtvDescriptorSize = 0;											// ��ȾĿ���������Ĵ�С
	UINT DsvDescriptorSize = 0;											// ���ģ���������Ĵ�С
	UINT CbvSrvUavDescriptorSize = 0;									// ������������������ɫ����Դ�������������ȡ�������Ĵ�С
};

struct Transform
{
	Transform(const DirectX::XMFLOAT3& t = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DirectX::XMFLOAT4& q = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		const DirectX::XMFLOAT3& s = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)) :
		Translation(t),
		Quaternion(q),
		Scale(s) {}

	DirectX::XMFLOAT3 GetForward()
	{
		DirectX::XMVECTOR quat = XMLoadFloat4(&Quaternion);
		auto mat = DirectX::XMMatrixRotationQuaternion(quat);
		DirectX::XMFLOAT3 forward;
		XMStoreFloat3(&forward, mat.r[2]);

		return forward;
	}

	DirectX::XMFLOAT3 GetRight()
	{
		DirectX::XMVECTOR quat = XMLoadFloat4(&Quaternion);
		auto mat = DirectX::XMMatrixRotationQuaternion(quat);
		DirectX::XMFLOAT3 right;
		XMStoreFloat3(&right, mat.r[0]);

		return right;
	}

	DirectX::XMFLOAT3 GetUp()
	{
		DirectX::XMVECTOR quat = XMLoadFloat4(&Quaternion);
		auto mat = DirectX::XMMatrixRotationQuaternion(quat);
		DirectX::XMFLOAT3 up;
		XMStoreFloat3(&up, mat.r[1]);

		return up;
	}

	DirectX::XMFLOAT3 Translation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 Quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 Scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
};

struct Int3
{
	Int3() :x(0), y(0), z(0) {}
	Int3(int _x, int _y, int _z) :x(_x), y(_y), z(_z) {}
	int x;
	int y;
	int z;
};

struct Int3_Hash
{
	size_t operator()(const Int3& int3) const
	{
		return std::hash<int>()(int3.x) ^ std::hash<int>()(int3.y) ^ std::hash<int>()(int3.z);
	}
};

struct Int3_Cmp
{
	bool operator()(const Int3& lhs, const Int3& rhs) const {
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}
};

struct RaycastHit
{
	std::string Name;
	float Dist;
	DirectX::XMFLOAT3 Point;
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
	float FalloffStart = 1.0f;                          // ���/�۹�
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// ƽ�й�/�۹�
	float FalloffEnd = 10.0f;                           // ���/�۹�
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // ���/�۹�
	float SpotPower = 64.0f;                            // �۹�
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

	int EnableShadow = 0;
	int EnableSsao = 0;
	DirectX::XMFLOAT2 Pad2;

	// ���� [0, NUM_DIR_LIGHTS) ��ƽ�й�
	// ���� [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) �ǵ��
	// ���� [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS) �Ǿ۹�
	Light Lights[MaxLights];
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
	Wireframe,

	Count
};

//===========================================================
//===========================================================
// ��������
//===========================================================
//===========================================================

extern DirectX::XMMATRIX TransformToMatrix(const Transform& transform); // ��һ��Transformת���ɾ�����ʽ
extern Transform RotateTransformLocal(const Transform transform, DirectX::XMFLOAT3 axis, float angle); // ��תһ��transform
DirectX::XMVECTOR QuaterionLookAtLH(DirectX::FXMVECTOR EyePosition, DirectX::FXMVECTOR FocusPosition, DirectX::FXMVECTOR UpDirection); // �����ӵ㣬Ŀ�����Ϸ�������Ԫ������������ϵ��

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
// ����
//===========================================================
//===========================================================

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif