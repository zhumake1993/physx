#pragma once

#include "Common/Camera.h"
#include "Manager/GameObjectManager.h"
#include "Manager/MeshRenderInstanceManager.h"
#include "Manager/TextureManager.h"
#include "Manager/MaterialManager.h"
#include "Manager/MeshManager.h"
#include "Manager/InputManager.h"

class Scene
{
	friend class SceneManager;
public:
	Scene();
	virtual ~Scene();

	virtual void Initialize();
	virtual void Update(const GameTimer& gt);
	virtual void PostUpdate(const GameTimer& gt);

protected:

	virtual void BuildManagers() = 0;
	virtual void BuildTextures() = 0;
	virtual void BuildMaterials() = 0;
	virtual void BuildMeshes() = 0;
	virtual void BuildGameObjects() = 0;

	void Pick(int sx, int sy);

public:

	// Camera
	std::shared_ptr<Camera> mMainCamera;									// �������
	
	// Manager
	std::shared_ptr<GameObjectManager> mGameObjectManager;					// ��Ϸ���������
	std::shared_ptr<MeshRenderInstanceManager> mMeshRenderInstanceManager;	// ��Ⱦʵ��������
	std::shared_ptr<TextureManager> mTextureManager;						// ���������
	std::shared_ptr<MaterialManager> mMaterialManager;						// ���ʹ�����
	std::shared_ptr<MeshManager> mMeshManager;								// ���������
	std::shared_ptr<InputManager> mInputManager;							// ���������

	// Render
	bool mIsWireframe = false;
	bool mIsDepthComplexityUseStencil = false;
	bool mIsDepthComplexityUseBlend = false;
	bool mIsCubeMap = true;
	bool mIsShadowMap = true;
	bool mIsSsao = true;

	// Filter
	bool mIsBlur = false;
	bool mIsSobel = false;

	// �Ƿ������ײ���߿�
	bool mIsDrawRigidbody = false;
};