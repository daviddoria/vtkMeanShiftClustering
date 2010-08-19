#include "vtkMeanShiftClustering.h"

#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkKdTreePointLocator.h>
#include <vtkIdList.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>

vtkStandardNewMacro(vtkMeanShiftClustering);

vtkMeanShiftClustering::vtkMeanShiftClustering()
{
    this->WindowRadius = 1.0;
    this->ConvergenceThreshold = 0.1;
    this->MaxIterations = 100;
    this->MinDistanceBetweenClusters = .1;
    
}

int vtkMeanShiftClustering::RequestData(vtkInformation *vtkNotUsed(request),
                                 vtkInformationVector **inputVector,
                                 vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

  this->ClusterId.resize(input->GetNumberOfPoints());
  
  //Create the tree
  vtkSmartPointer<vtkKdTreePointLocator> pointTree = 
      vtkSmartPointer<vtkKdTreePointLocator>::New();
  pointTree->SetDataSet(input);
  pointTree->BuildLocator();
 
  double currentCenter[3];
  double newCenter[3];
  std::cout << "vtkMeanShiftClustering: There are " << input->GetNumberOfPoints() << " input points." << std::endl;
  for(vtkIdType pointId = 0; pointId < input->GetNumberOfPoints(); pointId++)
    {
    double currentPoint[3];
    input->GetPoint(pointId, currentPoint);
  
    currentCenter[0] = currentPoint[0];
    currentCenter[1] = currentPoint[1];
    currentCenter[2] = currentPoint[2];
    unsigned int iter = 0;
  
    double difference[3]; //must be declared here because of loop structure
    do
      {
      vtkSmartPointer<vtkIdList> result = 
	  vtkSmartPointer<vtkIdList>::New();
    
      pointTree->FindPointsWithinRadius(this->WindowRadius, currentCenter, result);
      
      vtkSmartPointer<vtkPoints> windowPoints = 
	vtkSmartPointer<vtkPoints>::New();
      for(vtkIdType windowPointId = 0; windowPointId < result->GetNumberOfIds(); windowPointId++)
	{
	double p[3];
	input->GetPoint(result->GetId(windowPointId), p);
	windowPoints->InsertNextPoint(p);
	}
	
      //compute the center of mass of the points inside the window
      double newCenter[3];
      CenterOfMass(windowPoints, newCenter);
	
      vtkMath::Subtract(newCenter, currentCenter, difference);
      
      this->AssignBtoA(currentCenter, newCenter);
      
      iter++;
      }while((iter < this->MaxIterations) && (vtkMath::Norm(difference) > this->ConvergenceThreshold));

      vtkVector3d centerVector(currentCenter[0], currentCenter[1], currentCenter[2]);
	
      //if there are no clusters, add this one for sure
      if(this->ClusterCenters.size() == 0) 
	{
	this->ClusterCenters.push_back(centerVector);
	this->ClusterId[pointId] = 0;
	continue; //go to next input point
	}
	
      //check if this cluster center has already been found
      bool alreadyFound = false;
      for(unsigned int i = 0; i < this->ClusterCenters.size(); i++)
	{
	if(this->Distance(centerVector, this->ClusterCenters[i]) < this->MinDistanceBetweenClusters)
	  {
	  //already found this cluster, stop looking.
	  alreadyFound = true;
	  this->ClusterId[pointId] = i;
	  break;
	  }
	}
	
      if(!alreadyFound)
	{
	this->ClusterCenters.push_back(centerVector);
	this->ClusterId[pointId] = this->ClusterCenters.size() - 1;
	}
	
      }//end loop over every point
      
  //create the color map
  vtkSmartPointer<vtkLookupTable> colorLookupTable = 
    vtkSmartPointer<vtkLookupTable>::New();
  colorLookupTable->SetTableRange(0, this->ClusterCenters.size() - 1);
  colorLookupTable->Build();
 
  //generate the colors for each point based on the color map
  vtkSmartPointer<vtkUnsignedCharArray> colors = 
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetNumberOfComponents(3);
  colors->SetName ("Colors");
 
  for(int i = 0; i < input->GetNumberOfPoints(); i++)
    {
    double dcolor[3];
    colorLookupTable->GetColor(this->ClusterId[i], dcolor);
  
    unsigned char color[3];
    for(unsigned int j = 0; j < 3; j++)
      {
      color[j] = 255 * dcolor[j]/1.0;
      }
      
    colors->InsertNextTupleValue(color);
    }
  
  output->ShallowCopy(input);
  output->GetPointData()->SetScalars(colors);
  
  return 1;
}


double vtkMeanShiftClustering::Distance(vtkVector3d avec, vtkVector3d bvec)
{
  double a[3];
  double b[3];
  
  a[0] = avec.GetX();
  a[1] = avec.GetY();
  a[2] = avec.GetZ();
  
  b[0] = bvec.GetX();
  b[1] = bvec.GetY();
  b[2] = bvec.GetZ();
  
  return vtkMath::Distance2BetweenPoints(a,b);
}

void vtkMeanShiftClustering::CenterOfMass(vtkPoints* points, double* center)
{
  center[0] = 0.0;
  center[1] = 0.0;
  center[2] = 0.0;
    
  for(vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
  {
    double point[3];
    points->GetPoint(i, point);
    
    center[0] += point[0];
    center[1] += point[1];
    center[2] += point[2];
  }
  
  double numberOfPoints = static_cast<double>(points->GetNumberOfPoints());
  center[0] = center[0]/numberOfPoints;
  center[1] = center[1]/numberOfPoints;
  center[2] = center[2]/numberOfPoints;
}

void vtkMeanShiftClustering::AssignBtoA(double* a, double* b)
{
  a[0] = b[0];
  a[1] = b[1];
  a[2] = b[2];
}

void vtkMeanShiftClustering::DisplayPointAssociations(ostream &os)
{
  os << "Point cluster membership: " << std::endl;
  for(unsigned int i = 0; i < this->ClusterId.size(); i++)
    {
    std::cout << "Point " << i << " belongs to cluster " << this->ClusterId[i] << std::endl;
    }
}

void vtkMeanShiftClustering::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "WindowRadius: " << this->WindowRadius
		<< "ConvergenceThreshold: " << this->ConvergenceThreshold 
		<< "MaxIterations: " << this->MaxIterations 
		<< "MinDistanceBetweenClusters: " << this->MinDistanceBetweenClusters
		<< std::endl;

}
