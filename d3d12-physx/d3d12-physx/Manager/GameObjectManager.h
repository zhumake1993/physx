#pragma once

#include "GameObject.h"

class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	void Initialize();

	bool HasGameObject(std::string name);
	bool HasGameObject(std::shared_ptr<GameObject> gameObject);
	std::shared_ptr<GameObject> GetGameObject(std::string name);
	void AddGameObject(std::shared_ptr<GameObject> gameObject);
	void DeleteGameObject(std::string name);
	void DeleteGameObject(std::shared_ptr<GameObject> gameObject);

	void Update(const GameTimer& gt);

private:
	//

public:
	//

private:
	std::unordered_map<std::string, std::shared_ptr<GameObject>> mGameObjects;
};