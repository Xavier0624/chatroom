QT += core gui widgets


CONFIG += c++11


TARGET = chatroom_qt
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           qt_chat_client.cpp \
           chat_engine.cpp

HEADERS += mainwindow.h \
           qt_chat_client.h \
           chat_engine.h

LIBS += -lpthread