
```
 vtkNew<vtkOpenGLGPUVolumeRayCastMapper> volumeMapper;

#if 0
    vtkNew<vtkRTAnalyticSource> wavelet;
    wavelet->SetWholeExtent(-127, 128,
                            -127, 128,
                            -127, 128);
    wavelet->SetCenter(0.0, 0.0, 0.0);

    vtkNew<vtkImageCast> ic;
    ic->SetInputConnection(wavelet->GetOutputPort());
    ic->SetOutputScalarTypeToUnsignedChar();
    volumeMapper->SetInputConnection(ic->GetOutputPort());
```