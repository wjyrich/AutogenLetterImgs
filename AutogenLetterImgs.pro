#-------------------------------------------------
#
# Project created by QtCreator 2017-08-25T00:51:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutogenLetterImgs
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    graphicssceneex.cpp \
    graphicsviewex.cpp \
    text.cpp

HEADERS  += mainwindow.h \
    graphicssceneex.h \
    graphicsviewex.h \
    extcolordefs.h \
    text.h

FORMS    += mainwindow.ui

target.path = $$PREFIX/bin
desktop.files = AutogenLetterImgs.desktop
desktop.path = $$PREFIX/share/applications/
icons.path = $$PREFIX/share/icons/hicolor/apps/
icons.files = AutogenLetterImgs.png

INSTALLS += target desktop icons
