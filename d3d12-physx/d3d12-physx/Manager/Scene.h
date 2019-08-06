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
	std::shared_ptr<Camera> mMainCamera;									// 主摄像机
	
	// Manager
	std::shared_ptr<GameObjectManager> mGameObjectManager;					// 游戏物体管理器
	std::shared_ptr<MeshRenderInstanceManager> mMeshRenderInstanceManager;	// 渲染实例管理器
	std::shared_ptr<TextureManager> mTextureManager;						// 纹理管理器
	std::shared_ptr<MaterialManager> mMaterialManager;						// 材质管理器
	std::shared_ptr<MeshManager> mMeshManager;								// 网格管理器
	std::shared_ptr<InputManager> mInputManager;							// 输入管理器

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

	// 是否绘制碰撞体线框
	bool mIsDrawRigidbody = false;
};