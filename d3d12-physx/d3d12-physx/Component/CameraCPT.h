#pragma once

#include "Common/d3dUtil.h"
#include "Common/Camera.h"

class CameraCPT
{
public:

	CameraCPT(const Transform& transform);
	virtual ~CameraCPT();

	void SetMainCamera();

	// 设置世界坐标
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// 设置四元数
	void SetQuaterion(const DirectX::XMFLOAT4& q);

	// 设置平截头
	void SetLens(float fovY, float aspect, float zn, float zf);

	// 设置平截头截取
	void SetFrustumCulling(bool st);

	void Update();

private:
	
	//

public:
	
	//

private:
	
	std::shared_ptr<Camera> mCamera;
};