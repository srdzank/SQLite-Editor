cmake_minimum_required(VERSION 3.16)
project(sqlite_visual_editor LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)
# If you're on Qt5 instead, swap the line above for:
# find_package(Qt5 REQUIRED COMPONENTS Core Sql Widgets)

# On Windows, this embeds resources/app_icon.ico directly into the .exe,
# so it shows up in Explorer/the taskbar even before the app runs - the
# app.qrc icon below only takes effect once QApplication starts.
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resources/winsqlite.rc")
endif()

add_executable(sqlite_visual_editor
    main.cpp
    Schema.cpp
    QueryModel.cpp
    TableNode.cpp
    DiagramView.cpp
    MainWindow.cpp
    SqlEditor.cpp
    QueryWorker.cpp
    CreateTableDialog.cpp
    CreateIndexDialog.cpp
    InsertRowDialog.cpp
    ExportDialog.cpp
    SettingsDialog.cpp
    app.qrc
    ${APP_ICON_RESOURCE_WINDOWS}
)
target_include_directories(sqlite_visual_editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sqlite_visual_editor PRIVATE Qt6::Core Qt6::Sql Qt6::Widgets)