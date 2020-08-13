FROM kitware/itk-js-vtk:latest

WORKDIR /

# Remove Current VTK and run Another one
ENV CMAKE_TOOLCHAIN_FILE=/emsdk_portable/emscripten/sdk/cmake/Modules/Platform/Emscripten.cmake
RUN rm -rf /VTK
RUN rm -rf /VTK-build
RUN git clone https://gitlab.kitware.com/vtk/VTK.git &&  \
    cd VTK &&   \
    git checkout b39f9bfb && \
    git submodule update --init --recursive &&   \
    cd ../ &&   \
    mkdir VTK-build &&   \
    mkdir VTK-install &&   \
    cd VTK-build &&   \
    cmake   -G Ninja     \
            -DCMAKE_BUILD_TYPE:STRING=Release     \
            -DBUILD_SHARED_LIBS:BOOL=OFF     \
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}     \
            -DCMAKE_INSTALL_PREFIX:PATH=/vtk-install-prefix     \
            -DVTK_ENABLE_LOGGING:BOOL=OFF     \
            -DVTK_ENABLE_WRAPPING:BOOL=OFF       \
            -DVTK_LEGACY_REMOVE:BOOL=ON     \
            -DVTK_OPENGL_USE_GLES:BOOL=ON     \
            -DVTK_USE_SDL2:BOOL=ON     \
            -DVTK_NO_PLATFORM_SOCKETS:BOOL=ON \
            -DVTK_MODULE_ENABLE_VTK_hdf5:STRING=NO      \
            -DCMAKE_INSTALL_PREFIX:PATH=/VTK-install      \
            ../VTK &&   \
            ninja install -j7 &&   find . -name '*.o' -delete &&   cd .. && chmod -R 777 VTK-build
WORKDIR /work