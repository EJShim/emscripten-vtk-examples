#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkOpenGLSphereMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSphereSource.h>
#include <vtkOpenGLShaderProperty.h>
#include <vtkOpenGLCamera.h>
#include <vtkMatrix4x4.h>
#include <fstream>
#include <iostream>
#include <vtkObject.h>
#include <IWOpenGLSphereMapper.h>



#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include "vtkSDL2OpenGLRenderWindow.h"
#include "vtkSDL2RenderWindowInteractor.h"
#else
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#endif



std::string GetStringFromFile(char* filepath){

	std::ifstream t(filepath);
	std::string result((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	return result;
}



int main(int, char *[])
{

	// vtkObject::GlobalWarningDisplayOff();
	
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();


	#ifdef EMSCRIPTEN
	vtkSmartPointer<vtkSDL2RenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkSDL2RenderWindowInteractor>::New();
	vtkSmartPointer<vtkSDL2OpenGLRenderWindow> renderWindow = vtkSmartPointer<vtkSDL2OpenGLRenderWindow>::New();
	#else
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	#endif
	renderWindow->AddRenderer(renderer);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
	renderer->SetBackground(.2, .1, .01);
	renderWindow->Render();

	std::string filePath = "../test.txt";



	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(3);
	sphereSource->Update();
	vtkSmartPointer<vtkPolyData> polydata = sphereSource->GetOutput();




	// Visualization vtkOpenGLPolyDataMapper
	#ifdef EMSCRIPTEN
	vtkSmartPointer<vtkOpenGLSphereMapper> mapper = vtkSmartPointer<vtkOpenGLSphereMapper>::New();

	#else
	vtkSmartPointer<IWOpenGLSphereMapper> mapper = vtkSmartPointer<IWOpenGLSphereMapper>::New();	
	#endif
	mapper->SetInputData(polydata);






	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetPointSize(10);



	renderer->AddActor(actor);
	renderer->ResetCamera();
	renderWindow->Render();

	std::cout << "Start" << std::endl;

	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}