

#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include <vtkOpenGLPolyDataMapper.h>
#include "vtkRenderer.h"
#include "vtkSDL2OpenGLRenderWindow.h"
#include "vtkSDL2RenderWindowInteractor.h"
#include <vtkCylinderSource.h>

#include <vtkPropPicker.h>

#include <emscripten.h>
#include <emscripten/html5.h>



// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
  public:
    static MouseInteractorStyle2* New();
    vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
      int* clickPos = this->GetInteractor()->GetEventPosition();


      // Pick from this location.
      vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();
      picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

      double* pos = picker->GetPickPosition();
      std::cout << "Pick position (world coordinates) is: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
      std::cout << "Picked actor: " << picker->GetActor() << std::endl;

      // std::cout << "Picked actor: " << picker->GetActor() << std::endl;
      // //Create a sphere
      // vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
      // sphereSource->SetCenter(pos[0], pos[1], pos[2]);
      // sphereSource->SetRadius(0.1);

      // //Create a mapper and actor
      // vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      // mapper->SetInputConnection(sphereSource->GetOutputPort());

      // vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
      // actor->SetMapper(mapper);


      // //this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetDefaultRenderer()->AddActor(actor);
      // this->GetDefaultRenderer()->AddActor(actor);
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

  private:

};

vtkStandardNewMacro(MouseInteractorStyle2);




// ----------------------------------------------------------------------------
// Static objects
// ----------------------------------------------------------------------------

static vtkSDL2OpenGLRenderWindow* renderWindow = vtkSDL2OpenGLRenderWindow::New();


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  // Create a renderer and interactor
  vtkNew<vtkRenderer> renderer;
  // renderWindow->SetMultiSamples(0);
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkSDL2RenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkSmartPointer<MouseInteractorStyle2> style = vtkSmartPointer<MouseInteractorStyle2>::New();
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetDefaultRenderer(renderer);

  // Create pipeline
  vtkNew<vtkConeSource> coneSource;
  coneSource->Update();

    // Create a sphere
    vtkSmartPointer<vtkCylinderSource> cylinderSource =
    vtkSmartPointer<vtkCylinderSource>::New();
    cylinderSource->SetCenter(0.0, 0.0, 0.0);
    cylinderSource->SetRadius(5.0);
    cylinderSource->SetHeight(7.0);
    cylinderSource->SetResolution(100);

  vtkNew<vtkOpenGLPolyDataMapper> mapper;  
  mapper->SetInputConnection(cylinderSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  
  std::cout << "Actor address: " << actor << std::endl;

  // Add the actors to the scene
  renderer->AddActor(actor);

  // Start rendering app
  renderer->SetBackground(.1, .3, .2);


  // Start event loop
  renderWindowInteractor->Start();

  // Exit
  renderWindow->Delete();
  return 0;
}
