#pragma once
#include "Traits.h"
#include "Mesh.h"
#include "SceneGraph.h"
class GameObject {
public:
	typedef typename std::shared_ptr<Material> MaterialPtr;
	typedef typename std::shared_ptr<Mesh> MeshPtr;
	typedef typename MathTraits::Matrix4 Matrix4;
	typedef typename std::shared_ptr<Matrix4> Matrix4Ptr;
	typedef typename SceneNode::SceneNodePtr SceneNodePtr;

	void SetMaterial(MaterialPtr material);
	void SetMesh(MeshPtr mesh);
	void SetSceneNode(SceneNodePtr node);
	MeshPtr GetMesh();
	MaterialPtr GetMaterial();
	Matrix4 GetWorldTransform();
	//void SetWorldTransform(const Matrix4 &transform);
private:
	std::shared_ptr<Material> mMaterial;
	std::shared_ptr<Mesh> mMesh;
	SceneNodePtr mSceneNode;
};