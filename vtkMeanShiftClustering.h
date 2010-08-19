// .NAME vtkMeanShiftClustering - Cluster points using the mean shift method.
// .SECTION Description

#ifndef __vtkMeanShiftClustering_h
#define __vtkMeanShiftClustering_h

#include "vtkPolyDataAlgorithm.h" //superclass
#include "vtkSmartPointer.h"
#include "vtkVector.h"

#include <vector>

class vtkMeanShiftClustering : public vtkPolyDataAlgorithm
{
  public:
    static vtkMeanShiftClustering *New();
    vtkTypeMacro(vtkMeanShiftClustering, vtkPolyDataAlgorithm);
    void PrintSelf(ostream &os, vtkIndent indent);
    void DisplayPointAssociations(ostream &os);
    
    vtkSetMacro(WindowRadius, double);
    vtkGetMacro(WindowRadius, double);
    
  protected:
    vtkMeanShiftClustering();
    ~vtkMeanShiftClustering(){}
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    
    
    double kernel(double x);
    
    void CenterOfMass(vtkPoints* points, double* center);
    void AssignBtoA(double* a, double* b);
    
  private:
    std::vector<int> ClusterId; //which cluster each point is associated with
    std::vector<vtkVector3d> ClusterCenters;
    
    double WindowRadius;
    double ConvergenceThreshold;
    unsigned int MaxIterations;
    double MinDistanceBetweenClusters;
    
    double Distance(vtkVector3d avec, vtkVector3d bvec);
};

#endif
