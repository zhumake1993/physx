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
	
	std::shared_ptr<GameObjectManager> mGameObjectManager;				// 游戏物体管理器
	std::shared_ptr<InstanceManager> mInstanceManager;					// 渲染实例管理器
	std::shared_ptr<TextureManager> mTextureManager;					// 纹理管理器
	std::shared_ptr<MaterialManager> mMaterialManager;					// 材质管理器
	std::shared_ptr<MeshManager> mMeshManager;							// 网格管理器
	std::shared_ptr<InputManager> mInputManager;						// 输入管理器
};