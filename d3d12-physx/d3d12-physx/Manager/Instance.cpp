#include "Instance.h"

Instance::Instance()
{
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<UploadBuffer<InstanceData>>(gD3D12Device.Get(), mInstanceDataCapacity, false));
	}
}

Instance::~Instance()
{
}

std::shared_ptr<Mesh> Instance::GetMesh()
{
	return mMesh;
}

void Instance::CalculateBoundingBox()
{
	auto vertices = (Vertex*)mMesh->VertexBufferCPU->GetBufferPointer();
	auto size = mMesh->VertexBufferByteSize / mMesh->VertexByteStride;
	BoundingBox::CreateFromPoints(mBounds, size, &vertices[0].Pos, sizeof(Vertex));
}

void Instance::AddInstanceData(const std::string& gameObjectName, const XMFLOAT4X4& world, const UINT& matIndex, const XMFLOAT4X4& texTransform,
	const bool receiveShadow)
{
	if (mInstanceCount == mInstanceDataCapacity) {
		// 应该进行扩容操作
		// 暂时不实现
		OutputMessageBox("Can not add new instance data!");
		return;
	}

	InstanceData instance;
	instance.World = world;
	XMMATRIX worldMatrix = XMLoadFloat4x4(&world);
	XMMATRIX inverseTransposeWorldMatrix = MathHelper::InverseTranspose(worldMatrix);
	XMStoreFloat4x4(&instance.InverseTransposeWorld, inverseTransposeWorldMatrix);
	instance.MaterialIndex = matIndex;
	instance.TexTransform = texTransform;
	instance.ReceiveShadow = receiveShadow ? 1 : 0;

	mInstances[gameObjectName] = instance;

	++mInstanceCount;
}

void Instance::UpdateInstanceData(const std::string& gameObjectName, const XMFLOAT4X4& world, const UINT& matIndex, const XMFLOAT4X4& texTransform,
	const bool receiveShadow)
{
	mInstances[gameObjectName].World = world;
	XMMATRIX worldMatrix = XMLoadFloat4x4(&world);
	XMMATRIX inverseTransposeWorldMatrix = MathHelper::InverseTranspose(worldMatrix);
	XMStoreFloat4x4(&mInstances[gameObjectName].InverseTransposeWorld, inverseTransposeWorldMatrix);
	mInstances[gameObjectName].MaterialIndex = matIndex;
	mInstances[gameObjectName].TexTransform = texTransform;
	mInstances[gameObjectName].ReceiveShadow = receiveShadow ? 1 : 0;
}

void Instance::UploadInstanceData()
{
	auto& uploadBuffer = mFrameResources[gCurrFrameResourceIndex];

	XMMATRIX view = gCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	mVisibleCount = 0;
	for (auto& p : mInstances) {
		XMMATRIX world = XMLoadFloat4x4(&p.second.World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
		XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

		// 将平截头从视坐标空间转换到物体的局部坐标空间，然后可以在局部坐标空间作相交性检测
		// 但是实际测试发现，如果物体的世界矩阵含有Scale分量，结果会出错
		// 所以这里采用在世界坐标空间内进行相交性检测
		//BoundingFrustum localSpaceFrustum;
		//camera->mCamFrustum.Transform(localSpaceFrustum, viewToLocal);

		// 将平截头从视坐标空间转换到世界坐标空间
		BoundingFrustum worldSpaceFrustum;
		gCamera->mCamFrustum.Transform(worldSpaceFrustum, invView);

		// 将包围盒从局部坐标空间转换到世界坐标空间
		BoundingBox boundingBoxW;
		mBounds.Transform(boundingBoxW, world);

		// 平截头剔除
		if ((worldSpaceFrustum.Contains(boundingBoxW) != DirectX::DISJOINT) || (gCamera->mFrustumCullingEnabled == false)) {

			XMMATRIX world = XMLoadFloat4x4(&p.second.World);
			XMMATRIX inverseTransposeWorld = XMLoadFloat4x4(&p.second.InverseTransposeWorld);
			XMMATRIX texTransform = XMLoadFloat4x4(&p.second.TexTransform);

			InstanceData instanceData;

			XMStoreFloat4x4(&instanceData.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&instanceData.InverseTransposeWorld, XMMatrixTranspose(inverseTransposeWorld));
			XMStoreFloat4x4(&instanceData.TexTransform, XMMatrixTranspose(texTransform));
			instanceData.MaterialIndex = p.second.MaterialIndex;
			instanceData.ReceiveShadow = p.second.ReceiveShadow;

			uploadBuffer->CopyData(mVisibleCount++, instanceData);
		}
	}
}

void Instance::Draw()
{
	gCommandList->IASetVertexBuffers(0, 1, &mMesh->VertexBufferView);
	gCommandList->IASetIndexBuffer(&mMesh->IndexBufferView);
	gCommandList->IASetPrimitiveTopology(mMesh->PrimitiveType);

	auto instanceBuffer = mFrameResources[gCurrFrameResourceIndex]->Resource();
	gCommandList->SetGraphicsRootShaderResourceView(0, instanceBuffer->GetGPUVirtualAddress());

	gCommandList->DrawIndexedInstanced(mMesh->IndexCount, mVisibleCount, 0, 0, 0);
}

bool Instance::Pick(FXMVECTOR rayOriginW, FXMVECTOR rayDirW, std::string& name, float& tmin, XMVECTOR& point)
{
	bool result = false;

	tmin = MathHelper::Infinity;

	for (auto& p : mInstances) {
		std::string gameObjectName = p.first;
		InstanceData& instanceData = p.second;

		XMMATRIX W = XMLoadFloat4x4(&instanceData.World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		// 将射线转换至物体的局部空间
		XMVECTOR rayOriginL = XMVector3TransformCoord(rayOriginW, invWorld);
		XMVECTOR rayDirL = XMVector3TransformNormal(rayDirW, invWorld);

		// 相交性测试需要单位化向量
		rayDirL = XMVector3Normalize(rayDirL);

		float dst = 0.0f;
		// 首先检测是否与包围盒相交
		if (mBounds.Intersects(rayOriginL, rayDirL, dst)) {

			auto vertices = (Vertex*)mMesh->VertexBufferCPU->GetBufferPointer();
			auto indices = (std::uint16_t*)mMesh->IndexBufferCPU->GetBufferPointer();
			UINT triCount = mMesh->IndexCount / 3;

			std::string nameL;
			float tminL = MathHelper::Infinity;
			XMVECTOR pointL;

			// 找到最近的交点
			for (UINT i = 0; i < triCount; ++i) {
				// 该三角形的索引
				UINT i0 = indices[i * 3 + 0];
				UINT i1 = indices[i * 3 + 1];
				UINT i2 = indices[i * 3 + 2];

				// 该三角形的顶点
				XMVECTOR v0 = XMLoadFloat3(&vertices[i0].Pos);
				XMVECTOR v1 = XMLoadFloat3(&vertices[i1].Pos);
				XMVECTOR v2 = XMLoadFloat3(&vertices[i2].Pos);

				// 迭代处理所有的三角形来找到最近的交点
				float t = 0.0f;
				if (TriangleTests::Intersects(rayOriginL, rayDirL, v0, v1, v2, t)) {

					if (t < tminL) {
						// 新的最近选取三角形
						tminL = t;
						
						// 计算相交点
						pointL = rayOriginL + rayDirL * tminL;

						// 最近选取物体的名字
						nameL = gameObjectName;
					}
				}
			}

			// 与mesh相交
			if (tminL < MathHelper::Infinity) {
				// 转换至世界空间
				XMVECTOR pointW = XMVector3TransformCoord(pointL, W);

				// 由于scale矩阵的存在，tminL不是实际的距离，因此需要使用两点间距离公式来计算实际距离
				float tminW = XMVectorGetX(XMVector3Length(gCamera->GetPosition() - pointW));

				if (tminW < tmin) {
					result = true;

					tmin = tminW;
					name = nameL;
					point = pointW;
				}
			}
		}
	}

	return result;
}