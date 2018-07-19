# This is the root ITK CMakeLists file.
cmake_minimum_required(VERSION 2.8.9)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()


# This project is designed to be built outside the Insight source tree.
project(SoftRender)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(SoftRender    	Application.cxx
			     	SoftRenderer.h
			     	SoftRenderer.cpp
			     	RGBColor.h
			     	RGBColor.cpp
			     	Material.h
			     	Material.cpp
			     	Sampler.h
			     	Sampler.cpp
			     	Quaternions.h
			     	Quaternions.cxx
			     	GeometryPipeline.h
			     	GeometryPipeline.cxx
			     	RasterizeStage.h
			     	RasterizeStage.cxx
			     	Traits.h
			     	Light.h
			     	Light.cxx
			     	Transform.h
			     	Transform.cxx
				Mesh.h
				Mesh.cxx
				SceneGraph.h
				SceneGraph.cxx
				Camera.h
				Camera.cxx
				BoundingVolume.h
				BoundingVolume.cxx
				GameObject.h
				GameObject.cxx
)

target_link_libraries(SoftRender ${ITK_LIBRARIES})