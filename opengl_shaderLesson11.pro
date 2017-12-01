#-------------------------------------------------
#
# Project created by QtCreator 2017-11-24T17:24:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opengl_shaderLesson11
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    qgleswidget.cpp \
    tool/mycamera.cpp \
    tool/programid.cpp \
    tool/program_p2_c4.cpp \
    dataBox/qdataboxopengl.cpp \
    dataBox/qdatatorus.cpp \
    shader/myshadercartoon.cpp \
    shader/myshadershadow.cpp \
    frameBuffer/qframebufferobject.cpp \
    shader/myshadersobel.cpp

HEADERS  += mainwindow.h \
    qgleswidget.h \
    tool/CELLMath.hpp \
    tool/mycamera.h \
    tool/programid.h \
    tool/program_p2_c4.h \
    dataBox/qdataboxopengl.h \
    dataBox/qdatatorus.h \
    shader/myshadercartoon.h \
    shader/myshadershadow.h \
    frameBuffer/qframebufferobject.h \
    shader/myshadersobel.h

FORMS    += mainwindow.ui
LIBS    += -lEGL -lfreeimage
