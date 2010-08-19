#include <vtkSmartPointer.h>
#include <vtkPointSource.h>
#include <vtkAppendPolyData.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>

#include "vtkMeanShiftClustering.h"


int main(int, char *[])
{
  
  vtkSmartPointer<vtkPointSource> pointSource1 = 
    vtkSmartPointer<vtkPointSource>::New();
  pointSource1->SetCenter(0,0,0);
  pointSource1->SetRadius(1);
  pointSource1->SetNumberOfPoints(40);
  pointSource1->Update();
  
  vtkSmartPointer<vtkPointSource> pointSource2 = 
    vtkSmartPointer<vtkPointSource>::New();
  pointSource2->SetCenter(5,5,5);
  pointSource2->SetRadius(1);
  pointSource2->SetNumberOfPoints(40);
  pointSource2->Update();
  
  vtkSmartPointer<vtkPointSource> pointSource3 = 
    vtkSmartPointer<vtkPointSource>::New();
  pointSource3->SetCenter(-5,5,0);
  pointSource3->SetRadius(1);
  pointSource3->SetNumberOfPoints(40);
  pointSource3->Update();
  
  vtkSmartPointer<vtkAppendPolyData> appendFilter = 
    vtkSmartPointer<vtkAppendPolyData>::New();
  appendFilter->AddInputConnection(pointSource1->GetOutputPort());
  appendFilter->AddInputConnection(pointSource2->GetOutputPort());
  appendFilter->AddInputConnection(pointSource3->GetOutputPort());
  appendFilter->Update();
  
  vtkSmartPointer<vtkMeanShiftClustering> meanShiftFilter =
      vtkSmartPointer<vtkMeanShiftClustering>::New();
  meanShiftFilter->SetInputConnection(appendFilter->GetOutputPort());
  meanShiftFilter->SetWindowRadius(1.5); //radius should be bigger than expected clusters
  meanShiftFilter->Update();
    
  //Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper = 
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(meanShiftFilter->GetOutputPort());
 
  vtkSmartPointer<vtkActor> actor = 
      vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(5);
 
  //Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer = 
      vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = 
      vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
 
  //Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(1,1,1); // Background color white
 
  //Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();
  
  return EXIT_SUCCESS;
}

