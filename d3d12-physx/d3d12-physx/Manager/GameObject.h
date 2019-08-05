#pragma once

#include "Common/d3dUtil.h"
#include "Common/GameTimer.h"

#include "Component/MeshRenderCPT.h"
#include "Component/RigidDynamicCPT.h"
#include "Component/RigidStaticCPT.h"

class GameObject
{
	friend class GameObjectManager;
public:
	GameObject(const Transform& transform, const std::string& name);
	virtual ~GameObject();

	virtual void Update(const GameTimer& gt);

	virtual void GetPicked(float dst, DirectX::XMFLOAT3 hitPoint);

	virtual void Release();

protected:

	// 键盘输入
	bool GetKeyDown(int key);
	bool GetKeyPress(int key);
	bool GetKeyUp(int key);

	// 游戏物体
	bool HasGameObject(std::string name);
	std::shared_ptr<GameObject> GetGameObject(std::string name);
	void AddGameObject(std::shared_ptr<GameObject> gameObject);
	void DeleteGameObject(std::string name);

	// 材质
	void AddMaterial();
	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> material);

	// 纹理
	int GetTextureIndex(std::string name);

	// 场景
	void SwitchScene(std::string name);

	// 定时删除
	void Destroy(float time);

public:
	std::string mName;
	Transform mTransform;

	std::shared_ptr<Material> mMaterial = nullptr;
	std::shared_ptr<MeshRenderCPT> mMeshRenderCPT = nullptr;
	std::shared_ptr<RigidDynamicCPT> mRigidDynamicCPT = nullptr;
	std::shared_ptr<RigidStaticCPT> mRigidStaticCPT = nullptr;

	std::shared_ptr<GameTimer> mGameTimer = nullptr;
	float mLifeTime = 0.0f;

private:

	bool mToBeDeleted = false;
};