#ifndef SoftRenderer_H
#define SoftRenderer_H

#include "itkMath.h"
#include <string>
#include <math.h>
#include "Material.h"
#include "Sampler.h"
#include "Quaternions.h"
#include <algorithm>
#include "GeometryPipeline.h"
#include "RasterizeStage.h"
#include "Traits.h"
#include "Light.h"
#include "SceneGraph.h"
#include "GameObject.h"
class SoftRenderer {

public:
	typedef typename MathTraits::Matrix4 Matrix4;
	typedef typename MathTraits::Matrix3 Matrix3;
	typedef typename MathTraits::Point4 PointType;
	typedef typename MathTraits::Vector3 VectorType;
	typedef typename RenderTraits::VBO VBO;
	typedef typename RenderTraits::IBO IBO;
	typedef typename RenderTraits::V2FBuffer V2FBuffer;
	typedef typename SceneNode::SceneNodePtr SceneNodePtr;
	typedef typename SceneNode::PVS PVS;
	//渲染图像的宽和高必须都是2的倍数。
	//这样才可以划分成2*2的块
	SoftRenderer(int w, int h, std::string outputFileName);
	~SoftRenderer();
	void CameraInit();
	void SetLight(const Light &light);
	void DrawTriangle();
	void OpenDepthTest();
	void SetAspect(float aspect);
	RasterizeStage & GetRasterizor();
	void UpdateVBO(const VBO &vbo);
	void UpdateIBO(const IBO &ibo);
	void RenderScene(Scene &scene);
private:
	Light mLight;
	std::shared_ptr<Sampler> mpNormalSampler;
	float mAspect;
	std::shared_ptr<GeometryPipeline>  mGeometryPipeline;
	std::shared_ptr<RasterizeStage>  mRasterizeStage;
	VBO mVBO;
	IBO mIBO;
};
#endif // !SoftRenderer_H




