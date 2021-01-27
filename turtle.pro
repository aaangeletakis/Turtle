TEMPLATE = app
CONFIG += console c++17 warn_on
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS -= -O1
QMAKE_CXXFLAGS *= -O2
#QMAKE_CXXFLAGS *= -O3 -march=native -flto

SOURCES += \
        main.cpp

HEADERS += \
    cmd_line.h \
    enum.h \
    global.h \
    graph.h \
    io.hpp \
    node.hpp \
    regex_macros_def.h \
    regex_macros_undef.h \
    token.hpp
DISTFILES += \
    PRAYER.txt \
    README.md \
    test.py
