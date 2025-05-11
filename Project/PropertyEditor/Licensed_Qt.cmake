if(MSVC)
	include(${c_RootThirdPartyDirPath}/Qt/Qt_5_8_0/Shared.cmake)
else()
	#This is required for Qt on Linux. cIf this option is not specified, the following error will occur:
	#/usr/include/x86_64-linux-gnu/qt5/QtCore/qglobal.h:1187:4: error: #error "You must build your code with position independent code if Qt was built with -reduce-relocations. " "Compile your code with -fPIC (-fPIE is not enough)."
	#1187 | #  error "You must build your code with position independent code if Qt was built with -reduce-relocations. "
	target_compile_options(${ModuleName} PRIVATE -fPIC)
	include(${c_RootThirdPartyDirPath}/Qt/Qt_5_12_8/Shared.cmake)
endif()
