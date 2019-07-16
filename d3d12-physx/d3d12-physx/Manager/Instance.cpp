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
		// Ӧ�ý������ݲ���
		// ��ʱ��ʵ��
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

		// ��ƽ��ͷ��������ռ�ת��������ľֲ�����ռ䣬Ȼ������ھֲ�����ռ����ཻ�Լ��
		// ����ʵ�ʲ��Է��֣������������������Scale��������������
		// ���������������������ռ��ڽ����ཻ�Լ��
		//BoundingFrustum localSpaceFrustum;
		//camera->mCamFrustum.Transform(localSpaceFrustum, viewToLocal);

		// ��ƽ��ͷ��������ռ�ת������������ռ�
		BoundingFrustum worldSpaceFrustum;
		gCamera->mCamFrustum.Transform(worldSpaceFrustum, invView);

		// ����Χ�дӾֲ�����ռ�ת������������ռ�
		BoundingBox boundingBoxW;
		mBounds.Transform(boundingBoxW, world);

		// ƽ��ͷ�޳�
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

		// ������ת��������ľֲ��ռ�
		XMVECTOR rayOriginL = XMVector3TransformCoord(rayOriginW, invWorld);
		XMVECTOR rayDirL = XMVector3TransformNormal(rayDirW, invWorld);

		// �ཻ�Բ�����Ҫ��λ������
		rayDirL = XMVector3Normalize(rayDirL);

		float dst = 0.0f;
		// ���ȼ���Ƿ����Χ���ཻ
		if (mBounds.Intersects(rayOriginL, rayDirL, dst)) {

			auto vertices = (Vertex*)mMesh->VertexBufferCPU->GetBufferPointer();
			auto indices = (std::uint16_t*)mMesh->IndexBufferCPU->GetBufferPointer();
			UINT triCount = mMesh->IndexCount / 3;

			std::string nameL;
			float tminL = MathHelper::Infinity;
			XMVECTOR pointL;

			// �ҵ�����Ľ���
			for (UINT i = 0; i < triCount; ++i) {
				// �������ε�����
				UINT i0 = indices[i * 3 + 0];
				UINT i1 = indices[i * 3 + 1];
				UINT i2 = indices[i * 3 + 2];

				// �������εĶ���
				XMVECTOR v0 = XMLoadFloat3(&vertices[i0].Pos);
				XMVECTOR v1 = XMLoadFloat3(&vertices[i1].Pos);
				XMVECTOR v2 = XMLoadFloat3(&vertices[i2].Pos);

				// �����������е����������ҵ�����Ľ���
				float t = 0.0f;
				if (TriangleTests::Intersects(rayOriginL, rayDirL, v0, v1, v2, t)) {

					if (t < tminL) {
						// �µ����ѡȡ������
						tminL = t;
						
						// �����ཻ��
						pointL = rayOriginL + rayDirL * tminL;

						// ���ѡȡ���������
						nameL = gameObjectName;
					}
				}
			}

			// ��mesh�ཻ
			if (tminL < MathHelper::Infinity) {
				// ת��������ռ�
				XMVECTOR pointW = XMVector3TransformCoord(pointL, W);

				// ����scale����Ĵ��ڣ�tminL����ʵ�ʵľ��룬�����Ҫʹ���������빫ʽ������ʵ�ʾ���
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