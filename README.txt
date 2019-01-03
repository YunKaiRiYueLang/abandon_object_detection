#-------------------------------------------------
#
# Project created with Qt 5.6.2
#
# Compiling BGSLibrary QT GUI with CMAKE
#
#-------------------------------------------------
# Qt 5.x 64-bit for Desktop (MSVC 2017)
#-------------------------------------------------

mkdir build

cd build

set OpenCV_DIR=C:\OpenCV3.2.0\build

cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 15 Win64" ..
