#pragma once

#include "Common/d3dUtil.h"
#include "Common/Camera.h"

class CameraCPT
{
public:

	CameraCPT(const Transform& transform);
	virtual ~CameraCPT();

	void SetMainCamera();

	// ������������
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// ������Ԫ��
	void SetQuaterion(const DirectX::XMFLOAT4& q);

	// ����ƽ��ͷ
	void SetLens(float fovY, float aspect, float zn, float zf);

	// ����ƽ��ͷ��ȡ
	void SetFrustumCulling(bool st);

	void Update();

private:
	
	//

public:
	
	//

private:
	
	std::shared_ptr<Camera> mCamera;
};