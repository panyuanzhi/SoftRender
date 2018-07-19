#include "SceneGraph.h"
#include "GameObject.h"
#include <assert.h>
SceneNode::SceneNode()
{
	mWorldTransform.SetIdentity();
}

void SceneNode::AddGameObject(GameObjectPtr go)
{
	mGO = go;
}

void SceneNode::AddChild(SceneNodePtr child)
{
	if (mGO) {
		//��Ϸ������ڣ�˵����Ҷ�ӽ�㡣
		//��Ҷ�ӽ��������ӽ�㣬Ҫ�ѵ�ǰ����Ϸ�������ơ�
		SceneNodePtr node(new SceneNode);
		node->AddGameObject(mGO);
		mChildren.push_back(node);
		//�д�ȷ��
		mGO.~shared_ptr();
	}
	mChildren.push_back(child);
}

void SceneNode::SetParent(SceneNodePtr parent)
{
	mParent = parent;
}

const BoundingSphere & SceneNode::GetBounding()
{
	return mWorldBounding;
}

SceneNode::GameObjectPtr SceneNode::GetGameObject()
{
	return mGO;
}

SceneNode::ChildrenVector & SceneNode::GetChildren()
{
	return mChildren;
}

SceneNode::Matrix4 SceneNode::GetWorldTransform()
{
	return mWorldTransform;
}

void SceneNode::ComputeBounding()
{
	assert(mGO);

	mWorldBounding.ComputeBounding(mGO->GetMesh()->GetVBO());
	Point4 center = mWorldBounding.GetCenter();
	center = mWorldTransform*center;
	mWorldBounding.SetCenter(center);
	
	if (mParent) {
		mParent->UpdateBounding();
	}
	
}

void SceneNode::Translate(float x, float y, float z)
{
	//���¾ֲ��任
	Vector3 translate = mLocalTransform.GetTranslate();
	translate[0] += x; translate[1] += y; translate[2] += z;
	mLocalTransform.SetTranslate(translate[0], translate[1], translate[2]);
	mLocalTransform.Update();

	//���°�Χ��
	Point4 center = mWorldBounding.GetCenter();
	center[0] += x; center[1] += y; center[2] += z;
	mWorldBounding.SetCenter(center);
	if (mParent) {
		mParent->UpdateBounding();
	}
	

	//��������任
	Matrix4 translateMatrix;
	translateMatrix.SetIdentity();
	translateMatrix[0][3] = x; translateMatrix[1][3] = y; translateMatrix[2][3] = z;
	mWorldTransform = translateMatrix*mWorldTransform;
	ChildrenVector::iterator it = mChildren.begin();
	ChildrenVector::iterator end = mChildren.end();
	for (; it != end; ++it) {
		UpdateChildrenTransform(mWorldTransform, (*it));
	}
	
}

Transform & SceneNode::GetLocalTransform()
{
	return mLocalTransform;
}

void SceneNode::UpdateBounding()
{
	BoundingSphere tmp;
	ChildrenVector::iterator it = mChildren.begin();
	ChildrenVector::iterator end = mChildren.end();
	for (; it != end; ++it) {
		tmp = BoundingSphere::Merge(tmp, (*it)->GetBounding());
	}
	mWorldBounding = tmp;
	if (mParent) {
		mParent->UpdateBounding();
	}
}

void SceneNode::UpdateChildrenTransform(Matrix4 transform, SceneNodePtr node)
{
	Matrix4 localTransform = node->GetLocalTransform().GetTransform();
	mWorldTransform = localTransform*transform;
	ChildrenVector::iterator it = mChildren.begin();
	ChildrenVector::iterator end = mChildren.end();
	for (; it != end; ++it) {
		UpdateChildrenTransform(mWorldTransform, (*it));
	}
}

Scene::Scene()
{
	mRoot = std::make_shared<SceneNode>();
}

Scene::SceneNodePtr Scene::GetRoot()
{
	return mRoot;
}

void Scene::SetLight(std::shared_ptr<Light> light)
{
	mLight = light;
}

void Scene::SetCamera(std::shared_ptr<Camera> camera)
{
	mCamera = camera;
}

Scene::CameraPtr Scene::GetCamera()
{
	return mCamera;
}

Scene::LightPtr Scene::GetLight()
{
	return mLight;
}

void Scene::GetPVS(PVS & pvs)
{
	ComputePVS(pvs, mRoot);
}

void Scene::ComputePVS(PVS & pvs, SceneNodePtr node)
{
	if (mCamera->OutSide(node->GetBounding())) {
		return;
	}
	GameObjectPtr go = node->GetGameObject();
	if (go) {
		pvs.push_back(go);
		//������Ϸ����˵���Ѿ���Ҷ�ӽ����
		return;
	}
	ChildrenVector children = node->GetChildren();
	ChildrenVector::iterator it = children.begin();
	ChildrenVector::iterator end = children.end();
	for (; it != end; ++it) {
		ComputePVS(pvs, *it);
	}
}
