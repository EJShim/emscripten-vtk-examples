

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
#include <vtkVector.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <vtkOBBTree.h>
#include <vtkOpenGLSphereMapper.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include "vtkSDL2OpenGLRenderWindow.h"
#include "vtkSDL2RenderWindowInteractor.h"
#else
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#endif


vtkSmartPointer<vtkActor> actor;
vtkSmartPointer<vtkOBBTree> obbTree;

// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorStyle2* New();
	vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown() override
	{
		// Forward events
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();


		int* clickPos = this->GetInteractor()->GetEventPosition();
		std::cout << clickPos[0] << "," << clickPos[1] << std::endl;
		vtkRenderer* renderer = this->GetDefaultRenderer();
		vtkCamera* camera = renderer->GetActiveCamera();
		

		//Compute display to world
		vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
		coordinate->SetCoordinateSystemToDisplay();
		coordinate->SetValue(clickPos[0], clickPos[1], 0);
		
		vtkVector3d worldPos(coordinate->GetComputedWorldValue(renderer));
		vtkVector3d cameraPos(camera->GetPosition());
		vtkVector3d pickVec;

		vtkMath::Subtract(worldPos.GetData(), cameraPos.GetData(), pickVec.GetData());
		pickVec.Normalize();

		double clippingLength = camera->GetClippingRange()[1] - camera->GetClippingRange()[0];
		double cosMax = cos(camera->GetViewAngle() * 0.5 * M_PI / 180.0);
		double maxLength = clippingLength / cosMax;

		vtkVector3d p1 = worldPos;


		vtkMath::MultiplyScalar(pickVec.GetData(), maxLength );
		vtkVector3d p2;
		vtkMath::Add(worldPos.GetData(), pickVec.GetData(), p2.GetData());

		//OBBTree Pick
		vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
		obbTree->IntersectWithLine(p1.GetData(), p2.GetData(), pts, NULL );


		if(pts->GetNumberOfPoints() == 0) return;

		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(pts->GetPoint(0));
		sphereSource->Update();


		vtkSmartPointer<vtkPolyData> polydata = sphereSource->GetOutput();

		
		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(polydata);

		vtkSmartPointer<vtkActor> pickActor = vtkSmartPointer<vtkActor>::New();
		pickActor->SetMapper(mapper);
		pickActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
		pickActor->GetProperty()->SetPointSize(10);


		renderer->AddActor(pickActor);		
	}
};
vtkStandardNewMacro(MouseInteractorStyle2);




// Handle mouse events
class PickerInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static PickerInteractorStyle* New();
	vtkTypeMacro(PickerInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnLeftButtonDown() override
	{
		// Forward events
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

		int* clickPos = this->GetInteractor()->GetEventPosition();


		// Pick from this location.
		vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();
		picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

		double* pos = picker->GetPickPosition();
		std::cout << "Pick position (world coordinates) is: " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
		std::cout << "Picked actor: " << picker->GetActor() << std::endl;
	}
};
vtkStandardNewMacro(PickerInteractorStyle);


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
	vtkSmartPointer<PickerInteractorStyle> style = vtkSmartPointer<PickerInteractorStyle>::New();
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
	cylinderSource->Update();

	//Initialize OBBTree
	obbTree = vtkSmartPointer<vtkOBBTree>::New();
	obbTree->SetDataSet(cylinderSource->GetOutput());
	obbTree->BuildLocator();

	vtkNew<vtkOpenGLPolyDataMapper> mapper;  
	mapper->SetInputData(cylinderSource->GetOutput());

	actor = vtkSmartPointer<vtkActor>::New();
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
