#pragma once

#include "Manager/GameObjectManager.h"
#include "Common/FrameResource.h"

class Wave :
	public GameObject
{
public:
	Wave();
	Wave(const Wave& rhs) = delete;
	Wave& operator=(const Wave& rhs) = delete;
	~Wave();

	int RowCount()const;
	int ColumnCount()const;
	int VertexCount()const;
	int TriangleCount()const;
	float Width()const;
	float Depth()const;

	// Returns the solution at the ith grid point.
	const DirectX::XMFLOAT3& Position(int i)const { return mCurrSolution[i]; }

	// Returns the solution normal at the ith grid point.
	const DirectX::XMFLOAT3& Normal(int i)const { return mNormals[i]; }

	// Returns the unit tangent vector at the ith grid point in the local x-axis direction.
	const DirectX::XMFLOAT3& TangentX(int i)const { return mTangentX[i]; }

	void SetWavesVB(ID3D12Device* device);
	void Update(float dt);
	void Disturb(int i, int j, float magnitude);

private:
	virtual void Update()override;

public:
	//

private:
	int mNumRows = 0;
	int mNumCols = 0;

	int mVertexCount = 0;
	int mTriangleCount = 0;

	// Simulation constants we can precompute.
	float mK1 = 0.0f;
	float mK2 = 0.0f;
	float mK3 = 0.0f;

	float mTimeStep = 0.0f;
	float mSpatialStep = 0.0f;

	std::vector<DirectX::XMFLOAT3> mPrevSolution;
	std::vector<DirectX::XMFLOAT3> mCurrSolution;
	std::vector<DirectX::XMFLOAT3> mNormals;
	std::vector<DirectX::XMFLOAT3> mTangentX;

	std::unique_ptr<FrameResource<Vertex>> mWavesVB;
};