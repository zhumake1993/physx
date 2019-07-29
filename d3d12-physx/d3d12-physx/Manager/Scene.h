#pragma once

#include "Common/Camera.h"
#include "Manager/GameObjectManager.h"
#include "Manager/InstanceManager.h"
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

	std::shared_ptr<GameObjectManager> GetGameObjectManager();
	std::shared_ptr<InstanceManager> GetInstanceManager();
	std::shared_ptr<TextureManager> GetTextureManager();
	std::shared_ptr<MaterialManager> GetMaterialManager();
	std::shared_ptr<MeshManager> GetMeshManager();
	std::shared_ptr<InputManager> GetInputManager();

	std::shared_ptr<Camera> GetMainCamera();

	virtual void Update(const GameTimer& gt);
	virtual void PostUpdate(const GameTimer& gt);

protected:

	virtual void BuildManagers() = 0;
	virtual void BuildTextures() = 0;
	virtual void BuildMaterials() = 0;
	virtual void BuildMeshes() = 0;
	virtual void BuildGameObjects() = 0;

	void Pick(int sx, int sy);

protected:
	
	std::shared_ptr<GameObjectManager> mGameObjectManager;				// ��Ϸ���������
	std::shared_ptr<InstanceManager> mInstanceManager;					// ��Ⱦʵ��������
	std::shared_ptr<TextureManager> mTextureManager;					// ���������
	std::shared_ptr<MaterialManager> mMaterialManager;					// ���ʹ�����
	std::shared_ptr<MeshManager> mMeshManager;							// ���������
	std::shared_ptr<InputManager> mInputManager;						// ���������

	std::shared_ptr<Camera> mMainCamera;								// �������
};