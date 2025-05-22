[English](Doc/English/README.md)

# Niflect 示例: 基于动态反射的属性编辑器框架

**NiflectSamplePropertyEditor** 是 **C++ 动态反射**的实用示例, 展示如何**自动绑定**数据类型与 UI 编辑控件, 如:

- 运行时编辑模块中的对象 (如 .so/.dll 中的对象)
- 无须头文件即可创建类型对应的属性控件
- 基于 **Niflect** 生成与获取反射元数据

**Niflect** 是一款 C++ 原生风格反射框架

- 通过集成在相应构建系统中的 **NiflectGenTool** 工具生成反射代码
- 解析声明式宏标签标记的类型或成员
- 在运行时初始化反射元数以实现动态反射

## 构建

```bat
git clone git@github.com:sainimu78/NiflectSamplePropertyEditor.git
cd NiflectSamplePropertyEditor
git submodule update --init --remote
```

### Windows

VS 2015+, 建议 VS 2022

Qt 5.8 的依赖已传至本仓库不必另外安装

```bat
cd Build\PropertyEditor\Windows
Generate.bat
Build.bat
::或打开 VS 操作 start DefaultBuild\PropertyEditor.sln, 须注意打开后不可升级平台工具集等项目配置
DefaultBuild\Debug\bin\PropertyEditor.exe
```

### Linux

建议 Ubuntu 20

须自行安装 Qt 5.12.8

```
cd Build/PropertyEditor/Linux
./Generate.sh
./Build.sh
./DefaultBuild/Debug/bin/PropertyEditor
```

## 主要功能演示

### 1. 对象成员自动绑定 UI 控件

![Basic_Reflection](Doc/Basic_Reflection.gif)

*工作流*

- 用 Niflect 的宏标签定义 C++ 类与成员
- 通过反射元数据自动绑定 UI 控件

### 2. 编辑与重置属性

![Edit_Reset_Print](Doc/Edit_Reset_Print.gif)

*交互操作*

- 通过生成的 UI 控件编辑属性
- 重置任意属性为初始状态

