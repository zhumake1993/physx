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

private:
	
	std::unordered_map<std::string, std::function<void()>> mSceneCreateLambda;
	std::shared_ptr<Scene> mCurrScene;
	std::string mNextScene;
};

//===========================================================
//===========================================================
// ·½±ãº¯Êý
//===========================================================
//===========================================================

extern void SwitchCurrScene(std::string name);

extern std::shared_ptr<Camera> GetCurrMainCamera();
extern void SetCurrMainCamera(std::shared_ptr<Camera> camera);

extern std::shared_ptr<GameObjectManager> GetCurrGameObjectManager();
extern std::shared_ptr<MeshRenderInstanceManager> GetCurrMeshRenderInstanceManager();
extern std::shared_ptr<TextureManager> GetCurrTextureManager();
extern std::shared_ptr<MaterialManager> GetCurrMaterialManager();
extern std::shared_ptr<MeshManager> GetCurrMeshManager();
extern std::shared_ptr<InputManager> GetCurrInputManager();

extern bool& GetCurrIsWireframe();
extern bool& GetCurrIsDepthComplexityUseStencil();
extern bool& GetCurrIsDepthComplexityUseBlend();
extern bool& GetCurrIsCubeMap();
extern bool& GetCurrIsShadowMap();
extern bool& GetCurrIsSsao();

extern bool& GetCurrIsBlur();
extern bool& GetCurrIsSobel();

extern bool& GetCurrIsDrawRigidbody();