

#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
//#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPropPicker.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkPolyDataNormals.h>

#ifdef EMSCRIPTEN
#include "vtkSDL2OpenGLRenderWindow.h"
#include "vtkSDL2RenderWindowInteractor.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#endif


#include <string>
#include <fstream>
#include <streambuf>


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
		
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
	
private:
	
};

vtkStandardNewMacro(MouseInteractorStyle2);



std::string GetStringFromFile(char* filepath){

	std::ifstream t(filepath);
	std::string result((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	return result;

}



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
	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	#endif

	renderWindowInteractor->SetRenderWindow(renderWindow);	
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	style->SetDefaultRenderer(renderer);	

	

	vtkSmartPointer<vtkXMLPolyDataReader> xmlreader =  vtkSmartPointer<vtkXMLPolyDataReader>::New();	
	xmlreader->SetFileName("./resources/sample1.vtp");
	xmlreader->Update();

	

	//Get Polydata
	vtkSmartPointer<vtkPolyData> polydata = xmlreader->GetOutput();
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalGenerator->SetInputData(polydata);
	normalGenerator->ComputePointNormalsOn();
	normalGenerator->ComputeCellNormalsOff();
	normalGenerator->Update();
	polydata = normalGenerator->GetOutput();


	//Mapper with shader?
	std::string fsCode = GetStringFromFile("./resources/shaders/fragment.glsl");
	std::cout << fsCode << std::endl;
	vtkNew<vtkOpenGLPolyDataMapper> mapper;
	mapper->SetFragmentShaderCode(fsCode.c_str());
	mapper->SetInputData(polydata);
	
	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	
	
	std::cout << "Actor address: " << actor << std::endl;
	
	// Add the actors to the scene
	renderer->AddActor(actor);
	
	// Start rendering app
	renderer->SetBackground(.1, .3, .2);
	
	
	renderWindow->Render();
	// Start event loop
	renderWindowInteractor->Start();
	
	// Exit
	renderWindow->Delete();
	return 0;
}
