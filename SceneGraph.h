#pragma once
#include "Traits.h"
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "BoundingVolume.h"

class GameObject;
class SceneNode {
public:
	typedef typename MathTraits::Point4 Point4;
	typedef typename MathTraits::Vector3 Vector3;
	typedef typename MathTraits::Matrix4 Matrix4;
	typedef typename std::shared_ptr<SceneNode> SceneNodePtr;
	typedef typename std::shared_ptr<Mesh> MeshPtr;
	typedef typename std::shared_ptr<GameObject> GameObjectPtr;
	//Potential Visiable Set
	typedef typename std::vector<GameObjectPtr> PVS;
	typedef typename std::vector<SceneNodePtr> ChildrenVector;

	SceneNode();
	void AddGameObject(GameObjectPtr go);
	void AddChild(SceneNodePtr child);
	void SetParent(SceneNodePtr parent);
	//void ComputePVS(PVS &pvs);
	const BoundingSphere& GetBounding();
	GameObjectPtr GetGameObject();
	ChildrenVector& GetChildren();
	Matrix4 GetWorldTransform();
	//只有叶子结点才调用ComputeBounding计算包围体
	void ComputeBounding();
	void Translate(float x, float y, float z);
	Transform & GetLocalTransform();
private:
	void UpdateBounding();
	void UpdateChildrenTransform(Matrix4 transform, SceneNodePtr node);
private:
	Transform mLocalTransform;
	Matrix4 mWorldTransform;

	SceneNodePtr mParent;
	std::vector<SceneNodePtr> mChildren;

	GameObjectPtr mGO;

	BoundingSphere mWorldBounding;
};

class Scene {
public:
	typedef typename SceneNode::SceneNodePtr SceneNodePtr;
	typedef typename std::shared_ptr<Camera> CameraPtr;
	typedef typename SceneNode::PVS PVS;
	typedef typename SceneNode::GameObjectPtr GameObjectPtr;
	typedef typename SceneNode::ChildrenVector ChildrenVector;
	typedef typename std::shared_ptr<Light> LightPtr;

	Scene();
	SceneNodePtr GetRoot();
	void SetLight(std::shared_ptr<Light> light);
	void SetCamera(std::shared_ptr<Camera> camera);
	CameraPtr GetCamera();
	LightPtr GetLight();
	void GetPVS(PVS &pvs);

private:
	void ComputePVS(PVS &pvs, SceneNodePtr node);
private:
	SceneNodePtr mRoot;
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<Light> mLight;
};