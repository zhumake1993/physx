#include "Wave.h"
#include <cassert>
#include <ppl.h>

Wave::Wave()
	:GameObject()
{
	mGameObjectName = "wave";
	mTranslation = XMFLOAT3(0.0f, -20.0f, 0.0f);
	mMatName = "water";
	XMStoreFloat4x4(&mTexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	mMeshName = "wave";
	mRenderLayer = (int)RenderLayer::Transparent;


	// 参数
	int m = 128;
	int n = 128; 
	float dx = 1.0f;
	float dt = 0.03f;
	float speed = 4.0f;
	float damping = 0.2f;



	mNumRows = m;
	mNumCols = n;

	mVertexCount = m * n;
	mTriangleCount = (m - 1) * (n - 1) * 2;

	mTimeStep = dt;
	mSpatialStep = dx;

	float d = damping * dt + 2.0f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);
	mK1 = (damping * dt - 2.0f) / d;
	mK2 = (4.0f - 8.0f * e) / d;
	mK3 = (2.0f * e) / d;

	mPrevSolution.resize(m * n);
	mCurrSolution.resize(m * n);
	mNormals.resize(m * n);
	mTangentX.resize(m * n);

	// Generate grid vertices in system memory.

	float halfWidth = (n - 1) * dx * 0.5f;
	float halfDepth = (m - 1) * dx * 0.5f;
	for (int i = 0; i < m; ++i) {
		float z = halfDepth - i * dx;
		for (int j = 0; j < n; ++j) {
			float x = -halfWidth + j * dx;

			mPrevSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
			mCurrSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
			mNormals[i * n + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			mTangentX[i * n + j] = XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}

	// 创建并添加mesh
	{
		// 顶点
		std::vector<Vertex> vertices(VertexCount());
		for (int i = 0; i < VertexCount(); ++i) {
			vertices[i].Pos = Position(i);
			vertices[i].Normal = Normal(i);
			vertices[i].TexC.x = 0.5f + vertices[i].Pos.x / Width();
			vertices[i].TexC.y = 0.5f - vertices[i].Pos.z / Depth();
		}
		// 索引
		std::vector<std::uint16_t> indices(3 * TriangleCount());
		assert(VertexCount() < 0x0000ffff);
		int m = RowCount();
		int n = ColumnCount();
		int k = 0;
		for (int i = 0; i < m - 1; ++i) {
			for (int j = 0; j < n - 1; ++j) {
				indices[k] = i * n + j;
				indices[k + 1] = i * n + j + 1;
				indices[k + 2] = (i + 1) * n + j;

				indices[k + 3] = (i + 1) * n + j;
				indices[k + 4] = i * n + j + 1;
				indices[k + 5] = (i + 1) * n + j + 1;

				k += 6;
			}
		}
		gMeshManager->AddMesh("wave", vertices, indices);
	}

	mReceiveShadow = false;
}

Wave::~Wave()
{
}

int Wave::RowCount()const
{
	return mNumRows;
}

int Wave::ColumnCount()const
{
	return mNumCols;
}

int Wave::VertexCount()const
{
	return mVertexCount;
}

int Wave::TriangleCount()const
{
	return mTriangleCount;
}

float Wave::Width()const
{
	return mNumCols * mSpatialStep;
}

float Wave::Depth()const
{
	return mNumRows * mSpatialStep;
}

void Wave::SetWavesVB(ID3D12Device* device)
{
	mWavesVB = std::make_unique<FrameResource<Vertex>>(device, VertexCount(), false);
}

void Wave::Update(float dt)
{
	static float t = 0;

	// Accumulate time.
	t += dt;

	// Only update the simulation at the specified time step.
	if (t >= mTimeStep) {
		// Only update interior points; we use zero boundary conditions.
		concurrency::parallel_for(1, mNumRows - 1, [this](int i)
			//for(int i = 1; i < mNumRows-1; ++i)
		{
			for (int j = 1; j < mNumCols - 1; ++j) {
				// After this update we will be discarding the old previous
				// buffer, so overwrite that buffer with the new update.
				// Note how we can do this inplace (read/write to same element) 
				// because we won't need prev_ij again and the assignment happens last.

				// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
				// Moreover, our +z axis goes "down"; this is just to 
				// keep consistent with our row indices going down.

				mPrevSolution[i * mNumCols + j].y =
					mK1 * mPrevSolution[i * mNumCols + j].y +
					mK2 * mCurrSolution[i * mNumCols + j].y +
					mK3 * (mCurrSolution[(i + 1) * mNumCols + j].y +
						mCurrSolution[(i - 1) * mNumCols + j].y +
						mCurrSolution[i * mNumCols + j + 1].y +
						mCurrSolution[i * mNumCols + j - 1].y);
			}
		});

		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f; // reset time

		//
		// Compute normals using finite difference scheme.
		//
		concurrency::parallel_for(1, mNumRows - 1, [this](int i)
			//for(int i = 1; i < mNumRows - 1; ++i)
		{
			for (int j = 1; j < mNumCols - 1; ++j) {
				float l = mCurrSolution[i * mNumCols + j - 1].y;
				float r = mCurrSolution[i * mNumCols + j + 1].y;
				float t = mCurrSolution[(i - 1) * mNumCols + j].y;
				float b = mCurrSolution[(i + 1) * mNumCols + j].y;
				mNormals[i * mNumCols + j].x = -r + l;
				mNormals[i * mNumCols + j].y = 2.0f * mSpatialStep;
				mNormals[i * mNumCols + j].z = b - t;

				XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i * mNumCols + j]));
				XMStoreFloat3(&mNormals[i * mNumCols + j], n);

				mTangentX[i * mNumCols + j] = XMFLOAT3(2.0f * mSpatialStep, r - l, 0.0f);
				XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&mTangentX[i * mNumCols + j]));
				XMStoreFloat3(&mTangentX[i * mNumCols + j], T);
			}
		});
	}
}

void Wave::Disturb(int i, int j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < mNumRows - 2);
	assert(j > 1 && j < mNumCols - 2);

	float halfMag = 0.5f * magnitude;

	// Disturb the ijth vertex height and its neighbors.
	mCurrSolution[i * mNumCols + j].y += magnitude;
	mCurrSolution[i * mNumCols + j + 1].y += halfMag;
	mCurrSolution[i * mNumCols + j - 1].y += halfMag;
	mCurrSolution[(i + 1) * mNumCols + j].y += halfMag;
	mCurrSolution[(i - 1) * mNumCols + j].y += halfMag;
}

void Wave::Update()
{
	// 每0.25秒产生一个随机的波浪
	static float t_base = 0.0f;
	if ((gTimer.TotalTime() - t_base) >= 0.25f) {
		t_base += 0.25f;

		int i = MathHelper::Rand(4, RowCount() - 5);
		int j = MathHelper::Rand(4, ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		Disturb(i, j, r);
	}

	// 更新波浪模拟
	Update(gTimer.DeltaTime());

	// 更新顶点缓冲
	auto currWavesVB = mWavesVB->GetCurrResource().get();
	for (int i = 0; i < VertexCount(); ++i) {
		Vertex v;

		v.Pos = Position(i);
		v.Normal = Normal(i);

		v.TexC.x = 0.5f + v.Pos.x / Width();
		v.TexC.y = 0.5f - v.Pos.z / Depth();

		currWavesVB->CopyData(i, v);
	}

	// 将波浪渲染项的动态顶点缓冲设置为当前帧的顶点缓冲
	auto mesh = gInstanceManager->mInstanceLayers[mRenderLayer][mMeshName]->GetMesh();

	mesh->VertexBufferGPU = currWavesVB->Resource();
	mesh->VertexBufferView.BufferLocation = mesh->VertexBufferGPU->GetGPUVirtualAddress();

	// 更新纹理转换矩阵，营造一种流动的感觉
	auto mat = gMaterialManager->GetMaterialData(mMatName);

	float& tu = mat.MatTransform(3, 0);
	float& tv = mat.MatTransform(3, 1);

	tu += 0.1f * gTimer.DeltaTime();
	tv += 0.02f * gTimer.DeltaTime();

	if (tu >= 1.0f)
		tu -= 1.0f;

	if (tv >= 1.0f)
		tv -= 1.0f;

	mat.MatTransform(3, 0) = tu;
	mat.MatTransform(3, 1) = tv;

	gMaterialManager->SetMaterialData(mMatName, mat);
}