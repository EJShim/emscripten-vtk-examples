#XMLPolyDDataReader is not working


Compile Warning : 
```
wasm-ld: warning: function signature mismatch: vtkexpat_XML_GetCurrentLineNumber
>>> defined as (i32) -> i32 in C:/sources/VTK-emscripten/build/lib/libvtkIOXMLParser-9.0.a(vtkXMLParser.cxx.o)
>>> defined as (i32) -> i64 in C:/sources/VTK-emscripten/build/lib/libvtkexpat-9.0.a(xmlparse.c.o)

wasm-ld: warning: function signature mismatch: vtkexpat_XML_GetCurrentColumnNumber
>>> defined as (i32) -> i32 in C:/sources/VTK-emscripten/build/lib/libvtkIOXMLParser-9.0.a(vtkXMLParser.cxx.o)
>>> defined as (i32) -> i64 in C:/sources/VTK-emscripten/build/lib/libvtkexpat-9.0.a(xmlparse.c.o)

wasm-ld: warning: function signature mismatch: vtkexpat_XML_GetCurrentByteIndex
>>> defined as (i32) -> i32 in C:/sources/VTK-emscripten/build/lib/libvtkIOXMLParser-9.0.a(vtkXMLParser.cxx.o)
>>> defined as (i32) -> i64 in C:/sources/VTK-emscripten/build/lib/libvtkexpat-9.0.a(xmlparse.c.o)
warning: undefined symbol: pthread_attr_setscope (referenced by top-level compiled C/C++ code)
```



Runtime Error :
```
ShaderObject.js:12804 exception thrown: RuntimeError: unreachable,RuntimeError: unreachable
    at signature_mismatch:vtkexpat_XML_GetCurrentByteIndex (http://localhost:8080/ShaderObject.wasm:wasm-function[489]:0x11495)
    at vtkXMLParser::GetXMLByteIndex() (http://localhost:8080/ShaderObject.wasm:wasm-function[4335]:0xb41d6)
    at vtkXMLDataParser::StartElement(char const*, char const**) (http://localhost:8080/ShaderObject.wasm:wasm-function[4246]:0xb05df)
    at vtkXMLParserStartElement (http://localhost:8080/ShaderObject.wasm:wasm-function[4318]:0xb39c6)
    at doContent (http://localhost:8080/ShaderObject.wasm:wasm-function[4386]:0xbc16e)
    at contentProcessor (http://localhost:8080/ShaderObject.wasm:wasm-function[4372]:0xb9782)
    at doProlog (http://localhost:8080/ShaderObject.wasm:wasm-function[4370]:0xb712e)
    at prologInitProcessor (http://localhost:8080/ShaderObject.wasm:wasm-function[4353]:0xb5340)
    at vtkexpat_XML_ParseBuffer (http://localhost:8080/ShaderObject.wasm:wasm-function[4366]:0xb680d)
    at vtkexpat_XML_Parse (http://localhost:8080/ShaderObject.wasm:wasm-function[4362]:0xb6342)

```
위는 windows 환경 emcmake 가 고장나서 그런듯....



그냥
Docker 사용하면 괜찮음

- prerequisites : docker
```
docker-compose up cmake
```
```
docker-compose up make
```


```
cd build && python -m http.server8080
```