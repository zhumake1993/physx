#pragma once

#include <functional>

#include "Scene.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void Initialize();

	void SwitchScene(std::string name);
	bool ChangingScene();
	void ChangeScene();

	void Update(const GameTimer& gt);

	std::shared_ptr<Scene> GetCurrScene();

	std::shared_ptr<GameObjectManager> GetCurrGameObjectManager();
	std::shared_ptr<InstanceManager> GetCurrInstanceManager();
	std::shared_ptr<TextureManager> GetCurrTextureManager();
	std::shared_ptr<MaterialManager> GetCurrMaterialManager();
	std::shared_ptr<MeshManager> GetCurrMeshManager();
	std::shared_ptr<InputManager> GetCurrInputManager();

	std::shared_ptr<Camera> GetCurrMainCamera();

private:
	
	std::unordered_map < std::string, std::function<void()> > mSceneCreateLambda;
	std::shared_ptr<Scene> mCurrScene;
	std::string mNextScene;
};