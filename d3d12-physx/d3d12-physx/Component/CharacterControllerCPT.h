#pragma once

#include "Common/d3dUtil.h"
#include "Component/MeshRenderCPT.h"

class CharacterControllerCPT
{
public:

	CharacterControllerCPT(const std::string& parentName, const Transform& transform);
	virtual ~CharacterControllerCPT();

	void AddCharacterController();

	int Move(DirectX::XMFLOAT3 disp, float minDist, float elapsedTime);

	void Update(Transform& transform);

	void Release();

private:
	
	//

public:
	
	std::string mName = "";
	std::string mParentName = "";
	Transform mTransform;

	float mContactOffset;
	float mStepOffset;
	float mSlopeLimit;
	float mRadius;
	float mHeight;
	DirectX::XMFLOAT3 mUpDirection;

private:
	
	std::shared_ptr<MeshRenderCPT> mMeshRenderCPT = nullptr;
};