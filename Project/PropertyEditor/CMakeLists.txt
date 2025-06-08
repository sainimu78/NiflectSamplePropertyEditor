cmake_minimum_required(VERSION 3.10.0)

include(../InitRootPath.cmake)

set(c_ProjectName PropertyEditor)

include(${c_RootCMakeProjectFrameworkDirPath}/ProjectBegin.cmake)
project(${c_ProjectName})
include(${c_RootCMakeProjectFrameworkDirPath}/ProjectEnd.cmake)


set(ModuleName ${c_ProjectName})

set(ModuleRootPath ${c_RootSourceDirPath}/${ModuleName})
set(ModuleSourcePath ${ModuleRootPath}/src)
set(ModuleIncludePath ${ModuleRootPath}/include)

set(IncludePathsPrivate "")
set(IncludePathsPublic "")
list(APPEND IncludePathsPrivate ${ModuleSourcePath})
list(APPEND IncludePathsPublic ${ModuleIncludePath})

file(GLOB_RECURSE ModuleSources ${ModuleSourcePath}/*.cpp ${ModuleSourcePath}/*.h)
create_source_group(${ModuleSourcePath} ${ModuleSources})
file(GLOB_RECURSE ModuleHeaders ${ModuleIncludePath}/*.h)
create_source_group(${ModuleIncludePath} ${ModuleHeaders})
set(SrcAll "")
list(APPEND SrcAll ${ModuleSources})
list(APPEND SrcAll ${ModuleHeaders})

add_executable(${ModuleName} ${SrcAll})

if (WIN32)
	set_target_properties(${ModuleName} PROPERTIES
		LINK_FLAGS "/SUBSYSTEM:WINDOWS"
	)
endif()

target_precompile_headers(${ModuleName} PUBLIC ${ModuleSourcePath}/PropertyEditorCommon.h)

target_include_directories(${ModuleName}
	PRIVATE ${IncludePathsPrivate}
	PUBLIC ${IncludePathsPublic}
)

list(APPEND v_ListAccessorSettingHeaderFilePath ${ModuleSourcePath}/AccessorSetting.h)
list(APPEND v_ListModuleHeaderFilePath ${ModuleHeaders})
set(v_EnabledLoadTimeModuleRegistrar TRUE)
#set(v_EnabledDebuggerAttaching TRUE)
include(${c_RootProjectDirPath}/Niflect/Default.cmake)

include(Licensed_Qt.cmake)

if(MSVC)
	set_target_properties(${ModuleName} PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG},${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}>"
	)
endif()


if(MSVC)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${c_ProjectName})
endif()

include(${c_RootCMakeProjectFrameworkDirPath}/Release.cmake)
include(${c_RootCMakeProjectFrameworkDirPath}/Cleanup.cmake)