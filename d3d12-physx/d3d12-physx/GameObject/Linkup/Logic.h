#pragma once

#include "Manager/GameObject.h"

class Logic :
	public GameObject
{
public:
	Logic(const std::string& name, const Transform& transform = Transform());
	~Logic();

	void PickCube(Int3 int3);

private:
	virtual void Update(const GameTimer& gt)override;

	void CreateCubes();
	void CreateSegments(const Int3& end);
	std::vector<DirectX::XMFLOAT3> CalRandColor();
	void Shuffle();
	bool IsLinkPair(Int3 a, Int3 b);
	void DeepFirstSearch(Int3 v);
	bool IsTurning(const Int3& curr, const Int3& next);

public:
	//

private:
	
	int mMapX;
	int mMapY;
	int mMapZ;
	int mNumOfLeft;
	Int3 mLastPick;
	std::unordered_map<Int3, int, Int3_Hash, Int3_Cmp> mCubeMap;
	std::unordered_map<Int3, std::shared_ptr<GameObject>, Int3_Hash, Int3_Cmp> mCubeMapping;

	std::unordered_map<Int3, bool, Int3_Hash, Int3_Cmp> mMarked;
	std::unordered_map<Int3, Int3, Int3_Hash, Int3_Cmp> mEdgeTo;
	std::unordered_map<Int3, int, Int3_Hash, Int3_Cmp> mTurns;
};