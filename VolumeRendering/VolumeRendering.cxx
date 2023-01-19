#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkImageCast.h>
#include <vtkImageShiftScale.h>


#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include "vtkSDL2OpenGLRenderWindow.h"
#include "vtkSDL2RenderWindowInteractor.h"
#else
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#endif

///////////////////////////////////////
#include <vtkImageData.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkPointData.h>
#include <vtkImageShiftScale.h>
#include <vtkThreadedImageAlgorithm.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkXMLImageDataReader.h>
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
	t->SetOutputScalarTypeToUnsignedShort();
	
	t->Update();
	
	imageData->ShallowCopy(t->GetOutput());
}


/////////////////////////////////////--------------



// ----------------------------------------------------------------------------
// Static objects
// ----------------------------------------------------------------------------


#ifdef EMSCRIPTEN
static vtkSDL2OpenGLRenderWindow* renderWindow = vtkSDL2OpenGLRenderWindow::New();
EM_BOOL resize_window(int eventType, const EmscriptenUiEvent* e, void* userData)
{
	renderWindow->SetSize(e->windowInnerWidth, e->windowInnerHeight);
	renderWindow->Render();
	return 0;
}
#else
static vtkRenderWindow* renderWindow = vtkRenderWindow::New();
#endif
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Main
// --------------------------------------------------------------
int main(int argc, char* argv[])
{	
	

	// Create a renderer and interactor
	vtkNew<vtkRenderer> renderer;
	renderWindow->AddRenderer(renderer);
	
	#ifdef EMSCRIPTEN
		vtkThreadedImageAlgorithm::SetGlobalDefaultEnableSMP(true);
		vtkNew<vtkSDL2RenderWindowInteractor> renderWindowInteractor;
	#else
		vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	#endif
	renderWindowInteractor->SetRenderWindow(renderWindow);
	
	vtkNew<vtkInteractorStyleTrackballCamera> style;
	renderWindowInteractor->SetInteractorStyle(style);
	style->SetDefaultRenderer(renderer);
	
	
	///Create Image Data test
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	// CreateImageData(imageData);
	
	
	
	//Reade ImageData
	vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
	reader->SetFileName("./resources/d2.vti");
	reader->Update();
	imageData = reader->GetOutput();    
	double* range = imageData->GetPointData()->GetScalars()->GetRange();
	std::cout << range[0] << "," << range[1] << std::endl;

	#ifdef EMSCRIPTEN
	vtkSmartPointer<vtkImageShiftScale> converter = vtkSmartPointer<vtkImageShiftScale>::New();
	converter->SetInputData(imageData);
	converter->SetShift(-range[0]);
	converter->SetScale(255.0/(range[1]-range[0]));
	converter->SetOutputScalarTypeToUnsignedChar();
	converter->Update();
	
	
	imageData = converter->GetOutput();
	range = imageData->GetPointData()->GetScalars()->GetRange();
	std::cout << range[0] << "," << range[1] << std::endl;
	#endif

	
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	volumeMapper->SetBlendModeToMaximumIntensity();
	volumeMapper->SetInputData(imageData);
	
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->ShadeOn();
	// volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);
	
	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(range[0], 0.0);
	compositeOpacity->AddPoint((range[1]-range[0]) / 10, 1.0);
	compositeOpacity->AddPoint(range[1], 1.0);
	volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.
	
	
	
	vtkSmartPointer<vtkColorTransferFunction> color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(range[0]  ,0.0,0.0,0.0);	
	color->AddRGBPoint((range[1]-range[0]) / 10, 1.0, 1.0, 1.0);
	color->AddRGBPoint(range[1] ,1.0,1.0,1.0);
	volumeProperty->SetColor(color);

	
	
	
	vtkSmartPointer<vtkVolume> volume =  vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	
	
	renderer->AddViewProp(volume);
	
	
	
	// Start rendering app
	renderer->SetGradientBackground(false);
	renderer->SetBackground(0.0, 0.1, 0.0);
	// renderWindow->SetSize(300, 300);
	
	// Attach Window Resize Callback
	
	#ifdef EMSCRIPTEN
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, resize_window);
	#endif
	
	renderWindow->Render();
	// Start event loop
	renderWindowInteractor->Start();
	
	// Exit
	renderWindow->Delete();
	return 0;
}
