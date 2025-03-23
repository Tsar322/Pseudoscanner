if(EXISTS "${CMAKE_SOURCE_DIR}/config.cmake")
    include("${CMAKE_SOURCE_DIR}/config.cmake")
endif()

cmake_minimum_required(VERSION 3.16)

project(Pseudoscanner_desktop VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Qml Core Gui)
find_package(OpenCV REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

set(APPLICATION_NAME "pseudoscanner")

qt_add_resources(RESOURCES ${CMAKE_SOURCE_DIR}/qml/resources.qrc)

qt_add_executable(${APPLICATION_NAME}
    src/main.cpp
    ${RESOURCES}
    src/image_provider.cpp
    src/app_logic.cpp
    src/image_data.cpp
    ../scanner_img_proc_lib/src/scanner_img_proc.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${OpenCV_INCLUDE_DIRS}
    ${Qt6Quick_INCUDE_DIRS}
    ${Qt6Qml_INCUDE_DIRS}
    ${Qt6Core_INCUDE_DIRS}
    ${Qt6Gui_INCUDE_DIRS}
	${Eigen3_INCLUDE_DIR}
    ../scanner_img_proc_lib/src
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(${APPLICATION_NAME} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${APPLICATION_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${APPLICATION_NAME}
    PUBLIC Qt6::Quick
    ${OpenCV_LIBS}
)

include(GNUInstallDirs)
install(TARGETS ${APPLICATION_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
