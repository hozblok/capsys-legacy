#-------------------------------------------------
#
# Project created by QtCreator 2012-09-24T01:10:19
#
#-------------------------------------------------

QT += core gui xml widgets concurrent

TARGET = capsys-legacy
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    coherence.cpp \
    randomforcap.cpp \
    dialogtreedata.cpp \
    sourceandenvir5t.cpp \
    screen.cpp \
    capillary.cpp \
    mathforcap.cpp \
    fft.cpp \
    dialogsetrangesetc.cpp \
    testfft.cpp \
    testsource.cpp

HEADERS += mainwindow.h \
    coherence.h \
    randomforcap.h \
    dialogtreedata.h \
    sourceandenvir5t.h \
    screen.h \
    capillary.h \
    mathforcap.h \
    fft.h \
    dialogsetrangesetc.h \
    testfft.h \
    testsource.h

FORMS += mainwindow.ui \
    dialogtreedata.ui \
    dialogsetrangesetc.ui

TRANSLATIONS += I18n_en.ts I18n_ru.ts

##############################

RESOURCES += \
    res.qrc

########## mathgl ############

unix:!macx:!win32: LIBS += -L$$PWD/mathgl/lin64/lib/ -lmgl -lmgl-qt -lmgl-qt5
win32: LIBS += -L$$PWD/mathgl/win32mgl2.4.2.1/lib/ -lmgl -lmgl-qt -lmgl-qt5

unix:!macx:!win32:INCLUDEPATH += $$PWD/mathgl/lin64/include
win32: INCLUDEPATH += $$PWD/mathgl/win32mgl2.4.2.1/include
unix:!macx:!win32:DEPENDPATH += $$PWD/mathgl/lin64/include
win32: DEPENDPATH += $$PWD/mathgl/win32mgl2.4.2.1/include

unix:!macx:!win32: PRE_TARGETDEPS += $$PWD/mathgl/lin64/lib/libmgl.a
win32: PRE_TARGETDEPS += $$PWD/mathgl/win32mgl2.4.2.1/lib/libmgl.a
unix:!macx:!win32: PRE_TARGETDEPS += $$PWD/mathgl/lin64/lib/libmgl-qt.a
win32: PRE_TARGETDEPS += $$PWD/mathgl/win32mgl2.4.2.1/lib/libmgl-qt.a
unix:!macx:!win32: PRE_TARGETDEPS += $$PWD/mathgl/lin64/lib/libmgl-qt5.a
win32: PRE_TARGETDEPS += $$PWD/mathgl/win32mgl2.4.2.1/lib/libmgl-qt5.a
