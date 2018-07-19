#include"GameObject.h"
void GameObject::SetMaterial(MaterialPtr material)
{
	mMaterial = material;
}

void GameObject::SetMesh(MeshPtr mesh)
{
	mMesh = mesh;
}

void GameObject::SetSceneNode(SceneNodePtr node)
{
	mSceneNode = node;
}

GameObject::MeshPtr GameObject::GetMesh()
{
	return mMesh;
}

GameObject::MaterialPtr GameObject::GetMaterial()
{
	return mMaterial;
}

GameObject::Matrix4 GameObject::GetWorldTransform()
{
	return mSceneNode->GetWorldTransform();
}

//void GameObject::SetWorldTransform(const Matrix4 & transform)
//{
//	mWorldTransform = transform;
//}