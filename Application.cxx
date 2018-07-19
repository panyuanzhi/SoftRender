#include <string>
#include <math.h>
#include "SoftRenderer.h"
#include "Sampler.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "Quaternions.h"
#include "Traits.h"
#include "Mesh.h"
#include "SceneGraph.h"
#include "GameObject.h"
typedef MeshTraits::ImageType ImageType;
typedef MeshTraits::MeshType MeshType;
typedef MeshType::PointType PointType;
typedef MeshType::CellType CellType;
typedef MeshType::CellsContainer::Iterator CellIterator;
typedef MeshTraits::TriangleCell TriangleCell;
typedef MeshTraits::CellAutoPointer CellAutoPointer;
typedef MeshTraits::VertexAttributionType VertexAttributionType;
typedef MathTraits::Vector3 Vector3;
typedef MathTraits::Matrix2 Matrix2;
typedef MathTraits::Matrix2x3 Matrix2x3;
typedef MathTraits::Point2 Point2;
typedef MathTraits::Point4 Point4;
typedef RenderTraits::VBO VBO;
typedef RenderTraits::IBO IBO;
typedef typename std::shared_ptr<Sampler> SamplerPtr;
typedef typename std::shared_ptr<GameObject> GameObjectPtr;
void WritePerlinNoise();
void RenderTest();
void WriteNormalMap(std::string filename);
void LightInit(Scene &scene);
void CameraInit(Scene &scene,float aspect);
SamplerPtr MainTextureInit();
SamplerPtr NormalTextureInit();
GameObjectPtr GameObjectInit();
int main() {
	//WritePerlinNoise();
	RenderTest();
	return 0;
}

void WritePerlinNoise()
{
	int length = 256;
	ImageType::Pointer image = ImageType::New();
	ImageType::IndexType start;
	start[0] = 0;
	start[1] = 0;
	ImageType::SizeType size;
	size[0] = length;
	size[1] = length;
	ImageType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	image->SetRegions(region);
	image->Allocate(true);

	typedef itk::ImageFileWriter<ImageType> WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName("PerlinNoise.png");
	writer->SetInput(image);
	Sampler sampler;
	for(int j=0;j<length;++j)
		for (int i = 0; i < length; ++i) {
			ImageType::IndexType index = { i,j };
			float x = i + 0.5;
			float y = j + 0.5;
			
			float noise = sampler.PerlinNoise3D(x, y, 0);
			ImageType::PixelType pixel;
			pixel[0] = noise * 255; pixel[1] = noise * 255; pixel[1] = noise * 255;
			image->SetPixel(index, pixel);
		}
	writer->Update();
}

void RenderTest()
{
	const int Image_Width = 1024;
	const int Image_Height = 768;
	std::string outputFileName = "Rasterization.png";

	SoftRenderer softRenderer(Image_Width, Image_Height, outputFileName);
	
	Scene scene;
	LightInit(scene);
	CameraInit(scene, (float)Image_Width / (float)Image_Height);

	GameObjectPtr cube = GameObjectInit();

	typedef SceneNode::SceneNodePtr SceneNodePtr;
	SceneNodePtr root = scene.GetRoot();
	SceneNodePtr node1(new SceneNode);
	root->AddChild(node1);
	node1->SetParent(root);
	node1->AddGameObject(cube);
	cube->SetSceneNode(node1);
	node1->ComputeBounding();
	node1->Translate(0.0, -10, 80);
	//typedef itk::Vector<float, 3> Vector3;
	//Vector3 src;
	//src[0] = 0; src[1] = 0; src[2] = 1;
	//Vector3 dst;
	//dst[0] = 0; dst[1] = 0; dst[2] = -1;
	////softRenderer.Rotation(src, dst);
	//softRenderer.Translate(0.0, -10, 80);

	softRenderer.RenderScene(scene);
}

void WriteNormalMap(std::string filename)
{
	MeshType::Pointer mesh = MeshType::New();
	MeshType::PointType p0, p1, p2;
	p0[0] = 20; p0[1] = 0; p0[2] = 50.0; p0[3] = 1.0;
	p1[0] = -20; p1[1] = 0.0; p1[2] = 50.0; p1[3] = 1.0;
	p2[0] = 0; p2[1] = 0; p2[2] = 0; p2[3] = 1.0;

	Vector3 p0p1, p0p2;
	p0p1[0] = p1[0] - p0[0]; p0p1[1] = p1[1] - p0[1]; p0p1[2] = p1[2] - p0[2];
	p0p2[0] = p2[0] - p0[0]; p0p2[1] = p2[1] - p0[1]; p0p2[2] = p2[2] - p0[2];
	Vector3 normal = itk::CrossProduct(p0p1, p0p2);
	normal.Normalize();

	VertexAttributionType attr0;
	attr0[0] = 1.0;
	attr0[1] = normal[0];
	attr0[2] = normal[1];
	attr0[3] = normal[2];
	attr0[8] = 1.0;
	attr0[9] = 0.0;

	VertexAttributionType attr1;
	attr1[0] = 1.0;
	attr1[1] = normal[0];
	attr1[2] = normal[1];
	attr1[3] = normal[2];
	attr1[8] = 0.0;
	attr1[9] = 0.0;

	VertexAttributionType attr2;
	attr2[0] = 1.0;
	attr2[1] = normal[0];
	attr2[2] = normal[1];
	attr2[3] = normal[2];
	attr2[8] = 0.5;
	attr2[9] = 1.0;

	//构建线性方程组C=A T.transpose
	//					B.transpose
	Matrix2 A;
	A[0][0] = attr1[8] - attr0[8]; A[0][1] = attr1[9] - attr0[9];
	A[1][0] = attr2[8] - attr0[8]; A[1][1] = attr2[9] - attr0[9];

	Matrix2x3 C;
	C[0][0] = p0p1[0]; C[0][1] = p0p1[1]; C[0][2] = p0p1[2];
	C[1][0] = p0p2[0]; C[1][1] = p0p2[1]; C[1][2] = p0p2[2];

	Matrix2x3 TB;
	TB = A.GetInverse()*C.GetVnlMatrix();

	attr0[10] = TB[0][0]; attr0[11] = TB[0][1]; attr0[12] = TB[0][2];//tangent
	attr0[13] = TB[1][0]; attr0[14] = TB[1][1]; attr0[15] = TB[1][2];//bitangent

	attr1[10] = TB[0][0]; attr1[11] = TB[0][1]; attr1[12] = TB[0][2];//tangent
	attr1[13] = TB[1][0]; attr1[14] = TB[1][1]; attr1[15] = TB[1][2];//bitangent

	attr2[10] = TB[0][0]; attr2[11] = TB[0][1]; attr2[12] = TB[0][2];//tangent
	attr2[13] = TB[1][0]; attr2[14] = TB[1][1]; attr2[15] = TB[1][2];//bitangent

	mesh->SetPoint(0, p0);
	mesh->SetPointData(0, attr0);
	mesh->SetPoint(1, p1);
	mesh->SetPointData(1, attr1);
	mesh->SetPoint(2, p2);
	mesh->SetPointData(2, attr2);

	CellAutoPointer triangle0;
	triangle0.TakeOwnership(new TriangleCell);
	triangle0->SetPointId(0, 0);
	triangle0->SetPointId(1, 1);
	triangle0->SetPointId(2, 2);
	mesh->SetCell(0, triangle0);

	CellIterator it = mesh->GetCells()->Begin();
	CellIterator end = mesh->GetCells()->End();
	for (; it != end; ++it) {
		MeshType::CellType * cellptr = it.Value();
		TriangleCell * cell = dynamic_cast<TriangleCell *>(cellptr);
		if (cell == ITK_NULLPTR)
		{
			continue;
		}
		const TriangleCell::PointIdentifier *pointsIds = cell->GetPointIds();
		PointType v0, v1, v2;
		mesh->GetPoint(pointsIds[0], &v0);
		mesh->GetPoint(pointsIds[1], &v1);
		mesh->GetPoint(pointsIds[2], &v2);
		VertexAttributionType attr0, attr1, attr2;
		mesh->GetPointData(pointsIds[0], &attr0);
		mesh->GetPointData(pointsIds[1], &attr1);
		mesh->GetPointData(pointsIds[2], &attr2);


	}
}

void LightInit(Scene &scene)
{
	std::shared_ptr<Light>  light(new Light);
	light->SetDirection(0, 1, 0);
	light->SetIrradiance(1.0, 1.0, 1.0);
	//renderer.SetLight(light);
	scene.SetLight(light);
}

void CameraInit(Scene & scene, float aspect)
{
	std::shared_ptr<Camera> camera(new Camera);
	camera->SetPosition(0, 0, 0);
	Vector3 R, U, D;
	R[0] = 1; R[1] = 0; R[2] = 0;
	U[0] = 0; U[1] = 1; U[2] = 0;
	D[0] = 0; D[1] = 0; D[2] = 1;
	camera->SetReferenceFrame(R, U, D);
	float top = 10;
	float bottom = -top;
	float width = (top - bottom) * aspect;
	float right = width / 2; 
	float left = -right;
	float n = 50; 
	float f = 5000;
	camera->SetViewVolume(right, left, top, bottom, n, f);
	camera->Update();
	scene.SetCamera(camera);
}

SamplerPtr MainTextureInit()
{
	SamplerPtr pSampler(new Sampler);
	//pSampler->ReadTexture("Penguins.jpg");
	//pSampler->SetLinearSample();
	pSampler->GenerateCheckerBoard();
	pSampler->GenerateMipmap();
	pSampler->SetNearestSample();
	return pSampler;
}

SamplerPtr NormalTextureInit()
{
	std::shared_ptr<Sampler> pNormalSampler(new Sampler);
	pNormalSampler->ReadTexture("brickwall_normal.jpg");
	pNormalSampler->SetLinearSample();
	return pNormalSampler;
}

GameObjectPtr GameObjectInit()
{
	std::shared_ptr<CubeMesh> cube(new CubeMesh);
	cube->Scale(3, 3, 3);
	SamplerPtr mainTexture = MainTextureInit();
	SamplerPtr normalTexture = NormalTextureInit();
	std::shared_ptr<Material> material(new Material(0, 1, 0, 1));
	material->SetMainTexture(mainTexture);
	material->SetNormalTexture(normalTexture);
	GameObjectPtr go(new GameObject);
	go->SetMesh(cube);
	go->SetMaterial(material);
	return go;
}
