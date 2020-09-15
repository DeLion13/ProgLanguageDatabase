#-------------------------------------------------
#
# Project created by QtCreator 2019-06-01T21:27:42
#
#-------------------------------------------------

QT       += core gui
QT += sql
QT += xml
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kursach
TEMPLATE = app

DEPENDPATH += . ../src/storage
INCLUDEPATH += ../src/storage

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        dialog.cpp \
        dialog_e.cpp \
        first_action.cpp \
        login.cpp \
        main.cpp \
        mainwindow.cpp \
        registration.cpp \
        sqlite_storage.cpp \
        xml_storage.cpp

HEADERS += \
        dialog.h \
        dialog_e.h \
        first_action.h \
        language.hpp \
        login.h \
        mainwindow.h \
        optional.h \
        programmers.hpp \
        registration.h \
        sqlite_storage.h \
        storage.hpp \
        user.h \
        xml_storage.h

FORMS += \
        dialog.ui \
        dialog_e.ui \
        first_action.ui \
        login.ui \
        mainwindow.ui \
        registration.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    data/sqlite/data.sqlite \
    data/sqlite/data_copy.sqlite \
    data/xml/lang.xml \
    data/xml/new_storage.xml \
