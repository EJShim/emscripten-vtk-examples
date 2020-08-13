

#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include <vtkOpenGLPolyDataMapper.h>
#include "vtkRenderer.h"
#include <vtkCylinderSource.h>
#include <vtkPropPicker.h>
#include <vtkCamera.h>
#include <vtkCoordinate.h>


#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include "vtkSDL2OpenGLRenderWindow.h"
#include "vtkSDL2RenderWindowInteractor.h"
#else
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#endif


// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorStyle2* New();
	vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown() override
	{
		int* clickPos = this->GetInteractor()->GetEventPosition();

		vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();


		int displayPos[3] = {clickPos[0], clickPos[1], 0};



		vtkRenderer* renderer = this->GetDefaultRenderer();


		//Compute display to world





		// std::cout << "Picked actor: " << picker->GetActor() << std::endl;

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

#ifdef EMSCRIPTEN
static vtkSDL2OpenGLRenderWindow* renderWindow = vtkSDL2OpenGLRenderWindow::New();
#else
static vtkRenderWindow* renderWindow = vtkRenderWindow::New();
#endif

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	// Create a renderer and interactor
	vtkNew<vtkRenderer> renderer;
	renderWindow->AddRenderer(renderer);

	#ifdef EMSCRIPTEN
	vtkNew<vtkSDL2RenderWindowInteractor> renderWindowInteractor;
	#else
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	#endif

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
