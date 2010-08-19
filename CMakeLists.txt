cmake_minimum_required(VERSION 2.6)

PROJECT(vtkMeanShiftClustering)
ENABLE_TESTING()

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(vtkMeanShiftClusteringExample vtkMeanShiftClusteringExample.cxx vtkMeanShiftClustering.cxx)
TARGET_LINK_LIBRARIES(vtkMeanShiftClusteringExample vtkHybrid vtkCharts)

#ADD_EXECUTABLE(vtkMeanShiftClusteringTest vtkMeanShiftClusteringTest.cxx vtkMeanShiftClustering.cxx)
#TARGET_LINK_LIBRARIES(vtkMeanShiftClusteringTest vtkHybrid )
#ADD_TEST(vtkMeanShiftClusteringTest vtkMeanShiftClusteringTest)
