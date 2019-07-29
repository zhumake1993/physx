#include "LinkupGameObjects.h"

using namespace DirectX;

extern XMVECTOR QuaterionLookAtLH(FXMVECTOR EyePosition, FXMVECTOR FocusPosition, FXMVECTOR UpDirection);

Logic::Logic(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
}

Logic::~Logic()
{
}

void Logic::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	if (GetKeyDown('X')) {

		// ≤‚ ‘œﬂ∂Œƒ£ƒ‚
		mLinePoints.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
		mLinePoints.push_back(XMFLOAT3(0.0f, 5.0f, 5.0f));
		mLinePoints.push_back(XMFLOAT3(5.0f, 5.0f, -1.0f));
		float radius = 0.1f;

		// ªÊπ’µ„
		for (int i = 0; i < mLinePoints.size(); i++) {
			Transform a;
			a.Translation = mLinePoints[i];
			a.Scale = XMFLOAT3(radius, radius, radius);

			auto inflection = std::make_shared<Inflection>("Inflection" + std::to_string(i), a);
			AddGameObject(inflection);
		}

		// ªÊ÷∆‘≤÷˘
		for (int i = 0; i < mLinePoints.size() - 1; i++) {
			XMVECTOR start = XMLoadFloat3(&mLinePoints[i]);
			XMVECTOR end = XMLoadFloat3(&mLinePoints[i + 1]);

			Transform a;
			XMStoreFloat3(&a.Translation, (start + end) / 2);
			auto q1 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PIDIV2);
			auto q2 = QuaterionLookAtLH(start, end, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			XMStoreFloat4(&a.Quaternion, XMQuaternionMultiply(q1, q2));
			a.Scale = XMFLOAT3(radius, XMVectorGetX(XMVector3Length(end - start)), radius);

			auto s1 = std::make_shared<Segment>("Segment" + std::to_string(i), a);
			AddGameObject(s1);
		}
	}
}
