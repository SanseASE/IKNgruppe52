TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    file_server.cpp \
    ../Source/Checksum.cpp \
    ../Source/lib.cpp \
    ../Source/Link.cpp \
    ../Source/Transport.cpp

HEADERS += \
    ../include/Checksum.h \
    ../include/lib.h \
    ../include/Link.h \
    ../include/TransConst.h \
    ../include/Transport.h

LIBS += -lezV24

CONFIG(debug, debug|release) {
DEFINES += DEBUG
}
