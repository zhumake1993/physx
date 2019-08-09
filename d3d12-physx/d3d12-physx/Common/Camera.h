#pragma once

#include "d3dUtil.h"

class Camera
{
public:

	Camera();
	~Camera();

	// 设置/获取Translation
	DirectX::XMVECTOR GetTranslation()const;
	DirectX::XMFLOAT3 GetTranslation3f()const;
	void SetTranslation(float x, float y, float z);
	void SetTranslation(const DirectX::XMFLOAT3& v);

	// 设置/获取Quaternion
	DirectX::XMVECTOR GetQuaternion()const;
	DirectX::XMFLOAT4 GetQuaternion4f()const;
	void SetQuaternion(float x, float y, float z, float w);
	void SetQuaternion(const DirectX::XMFLOAT4& v);

	// 设置平截头
	void SetLens(float fovY, float aspect, float zn, float zf);

	// 平截头截取
	DirectX::BoundingFrustum GetCamFrustum();
	bool GetFrustumCulling();
	void SetFrustumCulling(bool st);

	// 获取平截头参数
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// 获取远近平面的维度
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// 获取视矩阵和投影矩阵
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;
	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	// 修改位置和方位后重建视矩阵
	void UpdateViewMatrix();

private:

	DirectX::XMFLOAT3 mTranslation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 mQuaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	// 缓存平截头属性
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	// 平截头剔除
	DirectX::BoundingFrustum mCamFrustum;
	bool mFrustumCullingEnabled = true;

	// 缓存视矩阵和投影矩阵
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();
};