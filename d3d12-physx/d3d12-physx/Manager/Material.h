#pragma once

#include "Common/d3dUtil.h"

class Material
{
public:

	Material();
	~Material();

public:

	std::string mName = "";
	UINT mIndex = 0;

	DirectX::XMFLOAT4 mDiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };

	DirectX::XMFLOAT3 mFresnelR0 = { 0.01f, 0.01f, 0.01f };
	float mRoughness = 0.5f;

	DirectX::XMFLOAT4X4 mMatTransform = MathHelper::Identity4x4();

	DirectX::XMFLOAT4 mLerpDiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };

	UINT mDiffuseMapIndex = 0;
	UINT mNormalMapIndex = 0;
	float mLerpPara = 0;
};