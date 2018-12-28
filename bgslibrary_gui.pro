#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bgslibrary_gui
TEMPLATE = app

# For Windows x64 + Visual Studio 2015 + OpenCV 3.1.0
#INCLUDEPATH += C:/OpenCV3.1.0/build/include
#LIBS += -LC:/OpenCV3.1.0/build/x64/vc14/lib -lopencv_world310

# For Windows x64 + Visual Studio 2015 + OpenCV 3.2.0
#INCLUDEPATH += C:/OpenCV3.2.0/build/include
#LIBS += -LC:/OpenCV3.2.0/build/x64/vc14/lib -lopencv_world320
INCLUDEPATH+=D:/opencv341/opencv/build/include/opencv			 D:/opencv341/opencv/build/include/opencv2			 D:/opencv341/opencv/build/include
LIBS += -LD:/opencv341/opencv/build/x64/vc14/lib -lopencv_world341d
LIBS += -LD:/opencv341/opencv/build/x64/vc14/lib -lopencv_world341

# For Linux
# INCLUDEPATH += /usr/local/include/opencv
# LIBS += -L/usr/local/lib

RESOURCES = application.qrc

SOURCES += bgslibrary_gui.cpp\
    mainwindow.cpp \
    qt_utils.cpp \
    texteditor.cpp \
    detectabandon.cpp \
    BGModel.cpp \
    BGModelFuzzyGauss.cpp \
    LBFuzzyGaussian.cpp \

HEADERS  += mainwindow.h \
    qt_utils.h \
    texteditor.h \
    camtag.h \
    BGModel.h \
    BGModelFuzzyGauss.h \
    IBGS.h \
    LBFuzzyGaussian.h \
    detectabandon.h \
    Types.h

FORMS    += mainwindow.ui

DISTFILES += \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_1ch.i \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_3ch1t.i \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_3ch3t.i \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_s3ch.i \
    ../package_bgs/ViBe/LICENSE
