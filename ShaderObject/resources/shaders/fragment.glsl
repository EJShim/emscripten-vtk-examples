//VTK::System::Dec

uniform int PrimitiveIDOffset;
// VC position of this fragment
//VTK::PositionVC::Dec

// optional color passed in from the vertex shader, vertexColor
//VTK::Color::Dec

// optional surface normal declaration
//VTK::Normal::Dec

// extra lighting parameters
//VTK::Light::Dec

// Texture coordinates
//VTK::TCoord::Dec

// picking support
//VTK::Picking::Dec

// Depth Peeling Support
//VTK::DepthPeeling::Dec

// clipping plane vars
//VTK::Clip::Dec

// the output of this shader
//VTK::Output::Dec

// Apple Bug
//VTK::PrimID::Dec

// handle coincident offsets
//VTK::Coincident::Dec

//VTK::ZBuffer::Dec

void main()
{
  // VC position of this fragment. This should not branch/return/discard.
  //VTK::PositionVC::Impl

  // Place any calls that require uniform flow (e.g. dFdx) here.
  //VTK::UniformFlow::Impl

  // Set gl_FragDepth here (gl_FragCoord.z by default)
  //VTK::Depth::Impl

  // Early depth peeling abort:
  //VTK::DepthPeeling::PreColor

  // Apple Bug
  //VTK::PrimID::Impl

  //VTK::Clip::Impl

  //VTK::Color::Impl

  // Generate the normal if we are not passed in one
  //VTK::Normal::Impl

  //VTK::Light::Impl

  //VTK::TCoord::Impl


  // //Calcualte Depth, vertexVC is not normalized, need to use proper clipping range
  // float zNear = 270.0f;
  // float zFar = 280.0;
  // float vdf = 1.0f + (vertexVC.z + zNear) * (1.0f / (zFar-zNear));
  

  // gl_FragData[0] = vec4( ambientColor * ambient + diffuseL * diffuse + specularL * 1.0f , opacity);
  gl_FragData[0] = vec4(255.0f, 0.0f, 0.0f, 1.0f);
  if (gl_FragData[0].a <= 0.0)
  {
    discard;
  }

  //VTK::DepthPeeling::Impl

  //VTK::Picking::Impl

  // handle coincident offsets
  //VTK::Coincident::Impl

  //VTK::ZBuffer::Impl
}