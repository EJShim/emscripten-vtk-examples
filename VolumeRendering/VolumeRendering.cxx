/*=========================================================================
  Program:   Visualization Toolkit
  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkSDL2OpenGLRenderWindow.h"
#include "vtkSDL2RenderWindowInteractor.h"
#include <vtkOpenGLGPUVolumeRayCastMapper.h>

#include <emscripten.h>
#include <emscripten/html5.h>


///////////////////////////////////////
#include <vtkImageData.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkPointData.h>
#include <vtkImageShiftScale.h>
#include <vtkThreadedImageAlgorithm.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolume.h>


void CreateImageData(vtkImageData* imageData)
{
  // Create a spherical implicit function.
  vtkSmartPointer<vtkSphere> sphere = vtkSmartPointer<vtkSphere>::New();
  sphere->SetRadius(0.1);
  sphere->SetCenter(0.0,0.0,0.0);

  vtkSmartPointer<vtkSampleFunction> sampleFunction = vtkSmartPointer<vtkSampleFunction>::New();
  sampleFunction->SetImplicitFunction(sphere);
  sampleFunction->SetOutputScalarTypeToDouble();
  sampleFunction->SetSampleDimensions(127,127,127); // intentional NPOT dimensions.
  sampleFunction->SetModelBounds(-1.0,1.0,-1.0,1.0,-1.0,1.0);
  sampleFunction->SetCapping(false);
  sampleFunction->SetComputeNormals(false);
  sampleFunction->SetScalarArrayName("values");
  sampleFunction->Update();
    
  vtkDataArray* a = sampleFunction->GetOutput()->GetPointData()->GetScalars("values");
  double range[2];
  a->GetRange(range);

  vtkSmartPointer<vtkImageShiftScale> t =  vtkSmartPointer<vtkImageShiftScale>::New();
  t->SetInputConnection(sampleFunction->GetOutputPort());

  t->SetShift(-range[0]);
  double magnitude=range[1]-range[0];
  if(magnitude==0.0)
    {
    magnitude=1.0;
    }
  t->SetScale(255.0/magnitude);
  t->SetOutputScalarTypeToUnsignedChar();

  t->Update();
    
  imageData->ShallowCopy(t->GetOutput());
}


/////////////////////////////////////--------------



// ----------------------------------------------------------------------------
// Static objects
// ----------------------------------------------------------------------------

static vtkSDL2OpenGLRenderWindow* renderWindow = vtkSDL2OpenGLRenderWindow::New();

// ----------------------------------------------------------------------------

EM_BOOL resize_window(int eventType, const EmscriptenUiEvent* e, void* userData)
{
  renderWindow->SetSize(e->windowInnerWidth, e->windowInnerHeight);
  renderWindow->Render();
  return 0;
}

// ----------------------------------------------------------------------------
// Main
// --------------------------------------------------------------
int main(int argc, char* argv[])
{

  
  // Create a renderer and interactor
  vtkNew<vtkRenderer> renderer;
  // renderWindow->SetMultiSamples(0);
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkSDL2RenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetDefaultRenderer(renderer);


  ///Create Image Data test
  //Call this in Emscripten
  vtkThreadedImageAlgorithm::SetGlobalDefaultEnableSMP(true);
  vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
  CreateImageData(imageData);


  vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
  volumeMapper->SetBlendModeToComposite();
  volumeMapper->SetInputData(imageData);

  vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
  compositeOpacity->AddPoint(0.0,0.0);
  compositeOpacity->AddPoint(80.0,1.0);
  compositeOpacity->AddPoint(80.1,0.0);
  compositeOpacity->AddPoint(255.0,0.0);
  volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.


  
  vtkSmartPointer<vtkColorTransferFunction> color =  vtkSmartPointer<vtkColorTransferFunction>::New();
  color->AddRGBPoint(0.0  ,0.0,0.0,1.0);
  color->AddRGBPoint(40.0  ,1.0,0.0,0.0);
  color->AddRGBPoint(255.0,1.0,1.0,1.0);
  volumeProperty->SetColor(color);


  vtkSmartPointer<vtkVolume> volume =  vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);


  renderer->AddViewProp(volume);


  
  // Start rendering app
  renderer->SetBackground(0.2, 0.3, 0.4);
  renderer->SetBackground2(0.0, 0.0, 0.0);
  renderWindow->SetSize(300, 300);

  // Attach Window Resize Callback
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, resize_window);

  // Start event loop
  renderWindowInteractor->Start();

  // Exit
  renderWindow->Delete();
  return 0;
}
