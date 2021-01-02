TEMPLATE = app
CONFIG += console c++20 warn_on
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS -= -O1
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS *= -O3 -march=native -flto

SOURCES += \
        main.cpp

HEADERS += \
    global.h \
    io.hpp \
    node.hpp \
    token.hpp
DISTFILES += \
    README.md \
    test.py
