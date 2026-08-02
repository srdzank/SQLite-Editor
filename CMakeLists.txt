cmake_minimum_required(VERSION 3.16)
project(sqlite_visual_editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)
# Ako koristiš Qt5 umesto Qt6, otkomentariši liniju ispod:
# find_package(Qt5 REQUIRED COMPONENTS Core Sql Widgets)

# On Windows, embed resources/app_icon.ico directly into the .exe
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resources/winsqlite.rc")
endif()

# Dodat je WIN32 fleg kako se ne bi otvarao command prompt / konzola
add_executable(sqlite_visual_editor WIN32
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

# Eksplicitno postavljanje WIN32_EXECUTABLE svojstva za Windows GUI aplikaciju
set_target_properties(sqlite_visual_editor PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(sqlite_visual_editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sqlite_visual_editor PRIVATE Qt6::Core Qt6::Sql Qt6::Widgets)