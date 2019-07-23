#pragma once

#include "Scene.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void Initialize();

	void Update();

	std::shared_ptr<GameObjectManager> GetCurrGameObjectManager();
	std::shared_ptr<InstanceManager> GetCurrInstanceManager();
	std::shared_ptr<TextureManager> GetCurrTextureManager();
	std::shared_ptr<MaterialManager> GetCurrMaterialManager();
	std::shared_ptr<MeshManager> GetCurrMeshManager();
	std::shared_ptr<InputManager> GetCurrInputManager();

private:
	
	std::shared_ptr<Scene> mCurrScene;				// µ±Ç°³¡¾°
};