QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    README.md \
    TODO.md

win32: LIBS += -L$$PWD/../../../../../../../../mingw64/lib/ -lmodbus.dll

INCLUDEPATH += $$PWD/../../../../../../../../mingw64/include
DEPENDPATH += $$PWD/../../../../../../../../mingw64/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../../../../mingw64/lib/libmodbus.dll.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../../../../mingw64/lib/libmodbus.dll.a
