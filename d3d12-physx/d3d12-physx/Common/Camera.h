#pragma once

#include "d3dUtil.h"

class Camera
{
public:

	Camera();
	~Camera();

	// ����/��ȡTranslation
	DirectX::XMVECTOR GetTranslation()const;
	DirectX::XMFLOAT3 GetTranslation3f()const;
	void SetTranslation(float x, float y, float z);
	void SetTranslation(const DirectX::XMFLOAT3& v);

	// ����/��ȡQuaternion
	DirectX::XMVECTOR GetQuaternion()const;
	DirectX::XMFLOAT4 GetQuaternion4f()const;
	void SetQuaternion(float x, float y, float z, float w);
	void SetQuaternion(const DirectX::XMFLOAT4& v);

	// ����ƽ��ͷ
	void SetLens(float fovY, float aspect, float zn, float zf);

	// ƽ��ͷ��ȡ
	DirectX::BoundingFrustum GetCamFrustum();
	bool GetFrustumCulling();
	void SetFrustumCulling(bool st);

	// ��ȡƽ��ͷ����
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// ��ȡԶ��ƽ���ά��
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// ��ȡ�Ӿ����ͶӰ����
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;
	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	// �޸�λ�úͷ�λ���ؽ��Ӿ���
	void UpdateViewMatrix();

private:

	DirectX::XMFLOAT3 mTranslation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 mQuaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	// ����ƽ��ͷ����
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	// ƽ��ͷ�޳�
	DirectX::BoundingFrustum mCamFrustum;
	bool mFrustumCullingEnabled = true;

	// �����Ӿ����ͶӰ����
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();
};