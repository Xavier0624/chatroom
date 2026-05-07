# Qt 客户端源文件（独立使用本地的 chat_engine）
set(QT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    qt_chat_client.cpp
    qt_chat_client.h
    logindialog.cpp
    logindialog.h
    chat_engine.cpp          # qt 自己的引擎副本
)

add_executable(chatroom_qt ${QT_SOURCES})

# 让编译器在自己的 qt 目录下找到 chat_engine.h
target_include_directories(chatroom_qt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(chatroom_qt PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Threads::Threads
)

# 自动处理 moc
set_target_properties(chatroom_qt PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)