## libs & header directories

`Qt_5_x\Deps.cmake`

## (Failed) Build from source 5.15.16

https://download.qt.io/archive/qt/

https://download.qt.io/archive/qt/5.15/5.15.16/submodules/qtbase-everywhere-opensource-src-5.15.16.zip

Open cmd with VC native environment

```
%comspec% /k "D:\HenDuoDongXi\Vs2022\VC\Auxiliary\Build\vcvars64.bat"
```

go to the build directory

```
cd Qt_5_15_16\qtbase-everywhere-src-5.15.16\build
```

then

```
set path=..\..\flex-2.5.4a-1-bin\bin;%path%
..\configure -opensource -confirm-license -debug-and-release
nmake
```

## Build from source 6.8.1

https://download.qt.io/archive/qt/

https://download.qt.io/archive/qt/6.8/6.8.1/submodules/qtbase-everywhere-src-6.8.1.zip

Open cmd with VC native environment

```
%comspec% /k "D:\HenDuoDongXi\Vs2022\VC\Auxiliary\Build\vcvars64.bat"
```

then

```
..\configure -debug-and-release
cmake --build . --parallel
cmake --install
```

the `include` folder is located in the build folder

add following code to the linking target cmake

```
# Qt 6.8.1 requires
if(MSVC)
    #add_compile_options(/Zc:__cplusplus)
	target_compile_options(${ModuleName} PRIVATE /Zc:__cplusplus)
	target_compile_options(${ModuleName} PRIVATE /permissive-)
endif()
```

encounters error about "qt platform plugin ..."

### Ubuntu (Old experiment)

```
mkdir build
cd build
../configure -opensource -confirm-license -nomake examples -skip qtdeclarative -skip qtwebengine -skip qtwebsockets -skip qtdoc -debug
ninja
```

