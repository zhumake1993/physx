#pragma once

#include "Manager/Scene.h"

class Linkup : public Scene
{
public:

	Linkup();
	~Linkup();

	virtual void Initialize()override;

	virtual void Update(const GameTimer& gt)override;

private:

	virtual void BuildManagers()override;
	virtual void BuildTextures()override;
	virtual void BuildMaterials()override;
	virtual void BuildMeshes()override;
	virtual void BuildGameObjects()override;

	void MoveCamera(const GameTimer& gt);

private:

	POINT mLastMousePos;
};