cmake_minimum_required(VERSION 3.5)

project(EditorGUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure Qt 6 is detected
set(Qt6_DIR "D:/Qt/6.8.2/msvc2022_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Ensure QT_VERSION_MAJOR is set
set(QT_VERSION_MAJOR 6)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

set(QT_VERSION_MAJOR 6)  # Force define before usage
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(Core
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resource.qrc
        logger.h logger.cpp
        AutoComplete/AutoCompleteTextEdit.h AutoComplete/AutoCompleteTextEdit.cpp
        AutoComplete/SQLHighlighter.h AutoComplete/SQLHighlighter.cpp
        DBNavigator/CDBNavigator.h DBNavigator/CDBNavigator.cpp
        ERDiagram/CERDiagram.h ERDiagram/CERDiagram.cpp
        SQLWorkSpace/CSQLWorkSpace.h SQLWorkSpace/CSQLWorkSpace.cpp
        TableViewWorkspace/CTableViewWorkspace.h TableViewWorkspace/CTableViewWorkspace.cpp
        TimeLineWidget/customwidget.h TimeLineWidget/customwidget.cpp
        TimeLineWidget/customitemdelegate.h TimeLineWidget/customitemdelegate.cpp
        Video/QVideo.h Video/QVideo.cpp 
        TableEditorDialog/CTableEditorDialog.h TableEditorDialog/CTableEditorDialog.cpp
        TableDiagram/CTableDiagram.h TableDiagram/CTableDiagram.cpp
    )
else()
    if(ANDROID)
        add_library(Core SHARED ${PROJECT_SOURCES})
    else()
        add_executable(Core ${PROJECT_SOURCES})
    endif()
endif()

# Ensure the correct paths are included
target_include_directories(Core PRIVATE ../SQLParser/include)
target_link_libraries(Core PRIVATE Qt${QT_VERSION_MAJOR}::Widgets SQLParserLib)

# Fix macOS bundle identifier
if(QT_VERSION_MAJOR VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Core)
endif()

if(DEFINED BUNDLE_ID_OPTION)
    set_target_properties(Core PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
else()
    set_target_properties(Core PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
endif()

# Installation settings
include(GNUInstallDirs)
install(TARGETS Core
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Ensure qt_finalize_executable is called only for Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Core)
endif()

# Set runtime output directory
set_target_properties(Core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
