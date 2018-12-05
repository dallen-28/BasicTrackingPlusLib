cmake_minimum_required(VERSION 3.9)
 
PROJECT(BasicTrackingPlusLib)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(PlusLib REQUIRED)
include (${PlusLib_USE_FILE})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets Core Gui Xml REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.cxx)

# CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
add_executable(BasicTrackingPlusLib MACOSX_BUNDLE
${CXX_FILES} ${UISrcs} ${QT_WRAP})

target_link_libraries(BasicTrackingPlusLib ${VTK_LIBRARIES} ${ITK_LIBRARIES}
Qt5::Widgets
vtkGUISupportQt
vtkPlusServer
PlusWidgets
vtkPlusCommon
vtkPlusCalibration)
