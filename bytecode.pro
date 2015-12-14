TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    stack.cpp \
    vlist.cpp \
    scheme_std_lib.cpp

HEADERS += \
    stack.h \
    type.h \
    vlist.h \
    saveload.h \
    types.h


