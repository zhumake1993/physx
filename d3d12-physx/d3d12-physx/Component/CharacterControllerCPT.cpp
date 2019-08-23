#include "CharacterControllerCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

CharacterControllerCPT::CharacterControllerCPT(const std::string& parentName, const Transform& transform)
{
	mParentName = parentName;
	mTransform = transform;
}

CharacterControllerCPT::~CharacterControllerCPT()
{
	//
}

void CharacterControllerCPT::AddCharacterController()
{
	CPT::PxCapsuleControllerDesc desc;
	desc.position = PxFloat3(mTransform.Translation.x, mTransform.Translation.y, mTransform.Translation.z);
	desc.contactOffset = mContactOffset;
	desc.stepOffset = mStepOffset;
	desc.slopeLimit = mSlopeLimit;
	desc.radius = mRadius;
	desc.height = mHeight;
	desc.upDirection = PxFloat3(mUpDirection.x, mUpDirection.y, mUpDirection.z);

	mName = gPhysX.AddCharacterController(&desc);

	// 添加Mesh
	auto meshName = mParentName + "Rigidbody";

	if (!GetCurrMeshManager()->HasMesh(meshName)) {

		GeometryGenerator geoGen;
		GetCurrMeshManager()->AddMesh(meshName, geoGen.CreateCapsule(mRadius + mContactOffset, mHeight, 20, 10, 10));
	}

	// 添加MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(mTransform);
	mMeshRenderCPT->mMaterial = GetCurrMaterialManager()->GetDefaultMaterial();
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = meshName;
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Wireframe;
	mMeshRenderCPT->mReceiveShadow = false;
	mMeshRenderCPT->AddMeshRender();
}

int CharacterControllerCPT::Move(DirectX::XMFLOAT3 disp, float minDist, float elapsedTime)
{
	return gPhysX.MoveCharacterController(mName, PxFloat3(disp.x, disp.y, disp.z), minDist, elapsedTime);
}

void CharacterControllerCPT::Update(Transform& transform)
{
	auto translation = gPhysX.GetCharacterControllerTranslation(mName);
	mTransform.Translation = XMFLOAT3(translation.x, translation.y, translation.z);
	transform.Translation = mTransform.Translation;

	// 更新刚体MeshRender的世界坐标
	mMeshRenderCPT->Update(mTransform);
}

void CharacterControllerCPT::Release()
{
	gPhysX.DeleteCharacterController(mName);
}
