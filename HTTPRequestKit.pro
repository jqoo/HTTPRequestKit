#-------------------------------------------------
#
# Project created by QtCreator 2017-04-01T16:03:56
#
#-------------------------------------------------

QT       -= core gui

TARGET = HTTPRequestKit
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/../ThirdParty/include

LIBS += -lWs2_32 \
        -lUser32 \
        -lIphlpapi

SOURCES += \
    HTTPClient.cpp \
    HTTPError.cpp \
    HTTPRequest.cpp \
    HTTPResponse.cpp \
    pch.cpp \
    SSLCertificatePolicy.cpp

HEADERS += \
    targetver.h \
    HTTPClient.h \
    HTTPError.hpp \
    HTTPRequest.h \
    HTTPRequestKit.h \
    HTTPResponse.h \
    pch.h \
    SSLCertificatePolicy.hpp
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES +=
