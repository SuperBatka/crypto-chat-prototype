QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += "$$_PRO_FILE_PWD_/include"
win32:LIBS += "-L$$_PRO_FILE_PWD_/lib" -lgmpxx -lgmp

SOURCES += \
    deffiehellman.cpp \
    rsa.cpp \
    streebog.cpp \
    gost_mods.cpp\
    main.cpp \
    mainwindow.cpp

HEADERS += \
    deffiehellman.h \
    rsa.h \
    streebog.h\
    gost.h \
    gost_mods.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

QMAKE_LFLAGS_RELEASE += -static -static-libgcc
QMAKE_LFLAGS_DEBUG += -static -static-libgcc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
