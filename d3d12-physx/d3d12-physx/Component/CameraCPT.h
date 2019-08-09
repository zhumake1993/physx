#pragma once

#include "Common/d3dUtil.h"
#include "Common/Camera.h"

class CameraCPT
{
public:

	CameraCPT(const Transform& transform);
	virtual ~CameraCPT();

	// ����Ϊ�������
	void SetMainCamera();

	// ������������
	void SetTranslation(float x, float y, float z);
	void SetTranslation(const DirectX::XMFLOAT3& v);

	// ������Ԫ��
	void SetQuaterion(float x, float y, float z, float w);
	void SetQuaterion(const DirectX::XMFLOAT4& q);

	// ����ƽ��ͷ
	void SetLens(float fovY, float aspect, float zn, float zf);

	// ����ƽ��ͷ��ȡ
	void SetFrustumCulling(bool st);

	void Update(const Transform& transform);

private:
	
	//

public:
	
	//

private:
	
	std::shared_ptr<Camera> mCamera;
};