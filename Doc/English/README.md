# Niflect Sample: Property Editor Framework Based on Dynamic Reflection

**NiflectSamplePropertyEditor** is a practical demonstration of **dynamic reflection in C++**, showcasing how to **automatically bind** data types to UI editing controls. This enables:

- Editing objects from modules at runtime (e.g., objects from shared libraries/DLLs)
- Zero header dependencies between property controls and reflected types
- Generating and accessing reflection metadata based on **Niflect**

**Niflect** is a native-style C++ reflection framework that:

- Generates reflection metadata via **NiflectGenTool** (integrated into build systems)
- Parses reflected types and members (by declarative macro tags)
- Initializes reflection metadata at runtime (enabling dynamic reflection)

## Building

```bat
git clone git@github.com:sainimu78/NiflectSamplePropertyEditor.git
cd NiflectSamplePropertyEditor
git submodule update --init --remote
```

### Windows

VS 2015+, Recommended: VS 2022

Qt 5.8 dependencies are included in this repository - no additional installation required.

```bat
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

### 1. Automatic Binding Object Members to UI Controls

![Basic_Reflection](../Basic_Reflection.gif)
*Workflow:*

- Define C++ types (e.g., class with members) with Niflect macro tags
- Automatic UI control binding based on reflection metadata.

### 2. Editing and Resetting Properties

![Edit_Reset_Print](../Edit_Reset_Print.gif)
*Interactions:*

- Edit properties through generated UI controls
- Reset any property to its initial state

