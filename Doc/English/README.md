# Niflect Sample: Property Editor Framework

**NiflectSamplePropertyEditor** is a property editor framework that serves as an application sample of Niflect, demonstrating automatic binding between data types and UI editing controls through reflection.

**Niflect** is a native-style C++ reflection framework, that:

- Generates reflection metadata via **NiflectGenTool** (integrated into build systems)
- Uses macro tags to declare reflected types and fields

## Building

```bat
git clone git@github.com:sainimu78/NiflectSamplePropertyEditor.git
cd NiflectSamplePropertyEditor
git submodule update --init --remote
```

### Windows

VS 2015+, Recommended: VS 2022

Qt 5.8 dependencies are included in this repository - no additional installation required.

```
cd Build\PropertyEditor\Windows
Generate.bat
Build.bat
:: Alternatively, open the VS solution: start DefaultBuild\PropertyEditor.sln
:: Note: Do not upgrade platform toolset or project configurations when opening
DefaultBuild\Debug\bin\PropertyEditor.exe
```

### Linux

Recommended: Ubuntu 20

Qt 5.12.8 must be installed manually

```bash
cd Build/PropertyEditor/Linux
./Generate.sh
./Build.sh
./DefaultBuild/Debug/bin/PropertyEditor
```

## Key Demonstrations

### 1. Reflecting Object Members to UI Controls

![Basic_Reflection](../Basic_Reflection.gif)
*Workflow:*

- Define C++ classes/members with Niflect macro tags
- Automatic UI control binding based on reflection metadata.

### 2. Editing and Resetting Properties

![Edit_Reset_Print](../Edit_Reset_Print.gif)
*Interactions:*

- Edit properties through generated UI controls
- Reset any property to its initial state

