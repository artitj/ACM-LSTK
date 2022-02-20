cmake_minimum_required(VERSION 3.11)
project( LungNoduleSegmentater )
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )
find_package( VTK REQUIRED )
include( ${VTK_USE_FILE} )
if(VTK_QT_VERSION VERSION_GREATER "4")
  message("VTK was built with Qt support.")
  find_package(Qt5Widgets REQUIRED QUIET)
endif()
include_directories(../common)
add_executable( LungNoduleSegmentation
  itkLesionSegmentationCommandLineProgressReporter.cxx
  itkLesionSegmentationCommandLineProgressReporter.h
	LungNoduleSegmentation.cpp
	LesionSegmentationCLI.h
#	itkLungWallFeatureGenerator2.hxx
#	itkLungWallFeatureGenerator2.h
	../common/vtkCutPlaneWidget.h
	../common/vtkCutPlaneWidget.cxx
	../common/itkVTKViewImageAndSegmentation.cxx
	../common/itkVTKViewImageAndSegmentation.h)
target_link_libraries( LungNoduleSegmentation ${ITK_LIBRARIES} ${VTK_LIBRARIES})

