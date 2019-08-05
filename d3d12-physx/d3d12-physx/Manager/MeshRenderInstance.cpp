#include "MeshRenderInstance.h"
#include "Manager/CurrManagers.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

extern ComPtr<ID3D12Device> gD3D12Device;
extern ComPtr<ID3D12GraphicsCommandList> gCommandList;

MeshRenderInstance::MeshRenderInstance()
{
	mFrameResource = std::make_unique<FrameResource<MeshRenderData>>(gD3D12Device.Get(), mCapacity, false);
}

MeshRenderInstance::~MeshRenderInstance()
{
}

std::shared_ptr<Mesh> MeshRenderInstance::GetMesh()
{
	return mMesh;
}

void MeshRenderInstance::CalculateBoundingBox()
{
	auto vertices = (Vertex*)mMesh->VertexBufferCPU->GetBufferPointer();
	auto size = mMesh->VertexBufferByteSize / mMesh->VertexByteStride;
	BoundingBox::CreateFromPoints(mBounds, size, &vertices[0].Pos, sizeof(Vertex));
}

void MeshRenderInstance::AddMeshRender(const std::string& name, const XMFLOAT4X4& world, const UINT& matIndex, const XMFLOAT4X4& texTransform,
	const bool receiveShadow)
{
	if (mCount == mCapacity) {
		ThrowMyEx("Can not add new instance data!")
	}

	MeshRenderData meshRenderData;
	meshRenderData.World = world;
	XMMATRIX worldMatrix = XMLoadFloat4x4(&world);
	XMMATRIX inverseTransposeWorldMatrix = MathHelper::InverseTranspose(worldMatrix);
	XMStoreFloat4x4(&meshRenderData.InverseTransposeWorld, inverseTransposeWorldMatrix);
	meshRenderData.MaterialIndex = matIndex;
	meshRenderData.TexTransform = texTransform;
	meshRenderData.ReceiveShadow = receiveShadow ? 1 : 0;

	mMeshRenders[name] = meshRenderData;

	++mCount;
}

void MeshRenderInstance::DeleteMeshRender(const std::string& name)
{
	mMeshRenders.erase(name);
	--mCount;
}

void MeshRenderInstance::UpdateMeshRender(const std::string& name, const XMFLOAT4X4& world, const UINT& matIndex, const XMFLOAT4X4& texTransform,
	const bool receiveShadow)
{
	mMeshRenders[name].World = world;
	XMMATRIX worldMatrix = XMLoadFloat4x4(&world);
	XMMATRIX inverseTransposeWorldMatrix = MathHelper::InverseTranspose(worldMatrix);
	XMStoreFloat4x4(&mMeshRenders[name].InverseTransposeWorld, inverseTransposeWorldMatrix);
	mMeshRenders[name].MaterialIndex = matIndex;
	mMeshRenders[name].TexTransform = texTransform;
	mMeshRenders[name].ReceiveShadow = receiveShadow ? 1 : 0;
}

void MeshRenderInstance::UploadMeshRender()
{
	XMMATRIX view = GetCurrMainCamera()->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	mVisibleCount = 0;
	for (auto& p : mMeshRenders) {
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
		GetCurrMainCamera()->mCamFrustum.Transform(worldSpaceFrustum, invView);

		// ����Χ�дӾֲ�����ռ�ת������������ռ�
		BoundingBox boundingBoxW;
		mBounds.Transform(boundingBoxW, world);

		// ƽ��ͷ�޳�
		if ((worldSpaceFrustum.Contains(boundingBoxW) != DirectX::DISJOINT) || (GetCurrMainCamera()->mFrustumCullingEnabled == false)) {

			XMMATRIX world = XMLoadFloat4x4(&p.second.World);
			XMMATRIX inverseTransposeWorld = XMLoadFloat4x4(&p.second.InverseTransposeWorld);
			XMMATRIX texTransform = XMLoadFloat4x4(&p.second.TexTransform);

			MeshRenderData meshRenderData;

			XMStoreFloat4x4(&meshRenderData.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&meshRenderData.InverseTransposeWorld, XMMatrixTranspose(inverseTransposeWorld));
			XMStoreFloat4x4(&meshRenderData.TexTransform, XMMatrixTranspose(texTransform));
			meshRenderData.MaterialIndex = p.second.MaterialIndex;
			meshRenderData.ReceiveShadow = p.second.ReceiveShadow;

			mFrameResource->Copy(mVisibleCount++, meshRenderData);
		}
	}
}

void MeshRenderInstance::Draw()
{
	gCommandList->IASetVertexBuffers(0, 1, &mMesh->VertexBufferView);
	gCommandList->IASetIndexBuffer(&mMesh->IndexBufferView);
	gCommandList->IASetPrimitiveTopology(mMesh->PrimitiveType);

	auto passCB = mFrameResource->GetCurrResource();
	gCommandList->SetGraphicsRootShaderResourceView(0, passCB->GetGPUVirtualAddress());

	gCommandList->DrawIndexedInstanced(mMesh->IndexCount, mVisibleCount, 0, 0, 0);
}

bool MeshRenderInstance::Pick(FXMVECTOR rayOriginW, FXMVECTOR rayDirW, std::string& name, float& tmin, XMVECTOR& point)
{
	bool result = false;

	tmin = MathHelper::Infinity;

	for (auto& p : mMeshRenders) {
		std::string gameObjectName = p.first;
		MeshRenderData& meshRenderData = p.second;

		XMMATRIX W = XMLoadFloat4x4(&meshRenderData.World);
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
				float tminW = XMVectorGetX(XMVector3Length(GetCurrMainCamera()->GetPosition() - pointW));

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