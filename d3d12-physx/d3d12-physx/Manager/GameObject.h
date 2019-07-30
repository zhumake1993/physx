#pragma once

#include "Common/d3dUtil.h"
#include "Common/GameTimer.h"

#include "Manager/MaterialManager.h"
#include "Manager/MeshRender.h"
#include "Manager/RigidDynamic.h"
#include "Manager/RigidStatic.h"

class GameObject
{
	friend class GameObjectManager;
public:
	GameObject(const std::string& name, const Transform& transform);
	virtual ~GameObject();

	virtual void Update(const GameTimer& gt);

	virtual void GetPicked(float dst, DirectX::XMFLOAT3 hitPoint);


	void Release();

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
	std::shared_ptr<MaterialData> GetMaterial();
	void AddMaterial(const std::string& name, std::shared_ptr<MaterialData> materialData);

	// 场景
	void SwitchScene(std::string name);

	// 定时删除
	void Destroy(float time);

public:
	std::string mName;
	Transform mTransform;

	std::unique_ptr<MeshRender> mMeshRender = nullptr;
	std::unique_ptr<RigidDynamic> mRigidDynamic = nullptr;
	std::unique_ptr<RigidStatic> mRigidStatic = nullptr;

	std::unique_ptr<GameTimer> mGameTimer = nullptr;
	float mLifeTime = 0.0f;

private:

	bool mToBeDeleted = false;
};