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

## Features

- Implemented an industrial-grade property editing framework from scratch using Qt (only 2000 lines of code)
- Customizable serialization for any type (see `AccessorSetting.h`)
- Automatic UI control binding for arbitrary type members (see `CVector3Accessor`)
- Dependency-free property tree construction for any reflected type (see `BuildPropertyFromRwNode`)
- Industrial-grade editing capabilities (e.g., resetting to initial values) with minimal code (see `QPropertyTree::ResetBranch`)
- Customizable serialization format support (JSON example in `CJsonFormat::Write`)
- Decoupled data layer from UI layer (replaceable UI implementation, see `PropertyNode.h`)

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

## Property Editing FAQ

**Key implementation**: `QPropertyTree::MarkChangeFromUi`

### **Q1: Is this framework efficient?**

#### **A1: Not efficient.** A full industrial-grade implementation would likely require 20,000+ lines of code (vs. the current 2,000+), due to challenges such as:

##### **Property Layer**

- **Cascading validity checks**:
  - Input constraints (e.g., numeric ranges) must trigger updates across dependent properties.
  - Nested properties (e.g., `CVector3` components) require synchronized UI updates.
- **Handling transient edits**:
  - Temporary preview values (e.g., slider drags) should avoid bloating undo history.
- **Dynamic property hierarchies**:
  - Represent edits as a tree/graph to manage runtime-generated structures.
  - Balance validity checks with simplicity in property definitions.

##### **Control Layer**

- **Performance tradeoffs**:
  - Optimize UI responsiveness while allowing full tree rebuilds.
  - Update visible controls only.
  - Preserve UI state (focus, expanded nodes) after rebuilds.

##### **Multi-instance Synchronization**

- Edit identical properties across filtered or distributed instances.
- Minimize redundant validity updates.

------

### **Q2: Does the minimal codebase limit practicality?**

#### **A2: No.** This implementation focuses on demonstrating:

- **Core workflows**: Branch reconstruction, the foundation for advanced features.
- **Reflection-driven design**: Metadata enables broad applicability without static dependencies.

------

### **Q3: **Why is undo/redo not implemented? Is it difficult?**

#### **A3: Challenges exist at two levels:**

##### **Simple Approach**

- Save/Load full property tree snapshots.
- **Flaws**: High memory usage and redundant operations.

##### **Industrial Approach**

- **Incremental serialization**: Store only modified data (deltas).
- **Key hurdles**:
  - Hybrid command design:
    - Generic commands for simplicity.
    - Delta-based commands for efficiency.
  - Isolate undo logic from persistence workflows.
  - Support partial data synchronization (e.g., transmitting deltas).

These are **essential challenges** for production-ready undo systems.