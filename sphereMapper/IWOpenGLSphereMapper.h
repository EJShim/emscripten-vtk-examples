/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   IWOpenGLSphereMapper
 * @brief   draw spheres using imposters
 *
 * An OpenGL mapper that uses imposters to draw spheres. Supports
 * transparency and picking as well.
 */

#ifndef IWOpenGLSphereMapper_h
#define IWOpenGLSphereMapper_h

#include "vtkOpenGLPolyDataMapper.h"
#include "vtkRenderingOpenGL2Module.h" // For export macro

class VTKRENDERINGOPENGL2_EXPORT IWOpenGLSphereMapper : public vtkOpenGLPolyDataMapper
{
public:
  static IWOpenGLSphereMapper* New();
  vtkTypeMacro(IWOpenGLSphereMapper, vtkOpenGLPolyDataMapper);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Convenience method to set the array to scale with.
   */
  vtkSetStringMacro(ScaleArray);
  //@}

  //@{
  /**
   * This value will be used for the radius is the scale
   * array is not provided.
   */
  vtkSetMacro(Radius, float);
  vtkGetMacro(Radius, float);

  /**
   * This calls RenderPiece (twice when transparent)
   */
  void Render(vtkRenderer* ren, vtkActor* act) override;

  /**
   * allows a mapper to update a selections color buffers
   * Called from a prop which in turn is called from the selector
   */
  // void ProcessSelectorPixelBuffers(vtkHardwareSelector *sel,
  //   int propid, vtkProp *prop) override;

protected:
  IWOpenGLSphereMapper();
  ~IWOpenGLSphereMapper() override;

  /**
   * Create the basic shaders before replacement
   */
  void GetShaderTemplate(
    std::map<vtkShader::Type, vtkShader*> shaders, vtkRenderer* ren, vtkActor* act) override;

  /**
   * Perform string replacements on the shader templates
   */
  void ReplaceShaderValues(
    std::map<vtkShader::Type, vtkShader*> shaders, vtkRenderer* ren, vtkActor* act) override;

  /**
   * Set the shader parameters related to the Camera
   */
  void SetCameraShaderParameters(vtkOpenGLHelper& cellBO, vtkRenderer* ren, vtkActor* act) override;

  /**
   * Set the shader parameters related to the actor/mapper
   */
  void SetMapperShaderParameters(vtkOpenGLHelper& cellBO, vtkRenderer* ren, vtkActor* act) override;

  const char* ScaleArray;

  /**
   * Update the VBO to contain point based values
   */
  void BuildBufferObjects(vtkRenderer* ren, vtkActor* act) override;

  void RenderPieceDraw(vtkRenderer* ren, vtkActor* act) override;

  virtual void CreateVBO(vtkPolyData* poly, vtkIdType numPts, unsigned char* colors,
    int colorComponents, vtkIdType nc, float* sizes, vtkIdType ns, vtkRenderer* ren);

  // used for transparency
  bool Invert;
  float Radius;

private:
  IWOpenGLSphereMapper(const IWOpenGLSphereMapper&) = delete;
  void operator=(const IWOpenGLSphereMapper&) = delete;
};

#endif
