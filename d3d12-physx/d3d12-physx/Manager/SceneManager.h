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

	void Update();

	std::shared_ptr<GameObjectManager> GetCurrGameObjectManager();
	std::shared_ptr<InstanceManager> GetCurrInstanceManager();
	std::shared_ptr<TextureManager> GetCurrTextureManager();
	std::shared_ptr<MaterialManager> GetCurrMaterialManager();
	std::shared_ptr<MeshManager> GetCurrMeshManager();
	std::shared_ptr<InputManager> GetCurrInputManager();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	
	std::unordered_map < std::string, std::function<void()> > mSceneCreateLambda;
	std::shared_ptr<Scene> mCurrScene;				// µ±Ç°³¡¾°
	std::string mNextScene;
};