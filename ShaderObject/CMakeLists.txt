cmake_minimum_required(VERSION 3.13)
project(ShaderObject)


# -----------------------------------------------------------------------------
# Handle VTK dependency
# -----------------------------------------------------------------------------

find_package(VTK
  COMPONENTS
    FiltersSources      # VTK pipeline
    InteractionStyle    # Mouse handling
    RenderingOpenGL2    # For Rendering
    RenderingUI         # For SDL2 Window
    IOGeometry
    IOXML
)

if (NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
else()
  message (STATUS "VTK_VERSION: ${VTK_VERSION}")
endif ()

# -----------------------------------------------------------------------------
# Compile example code
# -----------------------------------------------------------------------------

add_executable(${CMAKE_PROJECT_NAME} ShaderObject.cxx)

target_link_libraries(${CMAKE_PROJECT_NAME}
  PRIVATE
    VTK::FiltersSources
    VTK::InteractionStyle
    VTK::RenderingOpenGL2
    VTK::RenderingUI
    VTK::IOGeometry
    VTK::IOXML
)



# -----------------------------------------------------------------------------
# WebAssembly build options
# -----------------------------------------------------------------------------
if(EMSCRIPTEN)
  set(emscripten_options)
  list(APPEND emscripten_options
    "--bind"
    "-g3"
    "-O3"
    "SHELL:-s EXPORT_NAME=vtkApp"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s DEMANGLE_SUPPORT=1"
    "SHELL:-s EMULATE_FUNCTION_POINTER_CASTS=0"
    "SHELL:-s ERROR_ON_UNDEFINED_SYMBOLS=0"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s USE_PTHREADS=0"
    "SHELL:-s WASM=1"
    "SHELL:--preload-file resources"
    "SHELL:-Wno-almost-asm"
  )


  target_compile_options(${CMAKE_PROJECT_NAME}
    PUBLIC
      ${emscripten_options}
  )

  target_link_options(${CMAKE_PROJECT_NAME}
    PUBLIC
      ${emscripten_options}
  )
endif()

# -----------------------------------------------------------------------------
# VTK modules initialization
# -----------------------------------------------------------------------------

vtk_module_autoinit(
  TARGETS  ${CMAKE_PROJECT_NAME}
  MODULES  ${VTK_LIBRARIES}
)

# -----------------------------------------------------------------------------
# Copy HTML to build directory
# -----------------------------------------------------------------------------


if(EMSCRIPTEN)
add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
      $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
  
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/resources"
      $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/resources
)
else()
add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME} 
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/resources"
      $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/resources
)
endif()
