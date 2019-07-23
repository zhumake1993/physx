#pragma once

#include "Manager/GameObjectManager.h"
#include "Manager/InstanceManager.h"
#include "Manager/TextureManager.h"
#include "Manager/MaterialManager.h"
#include "Manager/MeshManager.h"
#include "Manager/InputManager.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void Initialize();

	std::shared_ptr<GameObjectManager> GetGameObjectManager();
	std::shared_ptr<InstanceManager> GetInstanceManager();
	std::shared_ptr<TextureManager> GetTextureManager();
	std::shared_ptr<MaterialManager> GetMaterialManager();
	std::shared_ptr<MeshManager> GetMeshManager();
	std::shared_ptr<InputManager> GetInputManager();

	virtual void Update();

protected:

	virtual void BuildManagers() = 0;
	virtual void BuildTextures() = 0;
	virtual void BuildMaterials() = 0;
	virtual void BuildMeshes() = 0;
	virtual void BuildGameObjects() = 0;

protected:
	
	std::shared_ptr<GameObjectManager> mGameObjectManager;				// ��Ϸ���������
	std::shared_ptr<InstanceManager> mInstanceManager;					// ��Ⱦʵ��������
	std::shared_ptr<TextureManager> mTextureManager;					// ���������
	std::shared_ptr<MaterialManager> mMaterialManager;					// ���ʹ�����
	std::shared_ptr<MeshManager> mMeshManager;							// ���������
	std::shared_ptr<InputManager> mInputManager;						// ���������
};