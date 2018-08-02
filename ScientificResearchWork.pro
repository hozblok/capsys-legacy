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

#GSL:

#unix:!symbian|win32: LIBS += -L$$PWD/../../../../../usr/lib/ -lgslcblas

#INCLUDEPATH += $$PWD/../../../../../usr/include
#DEPENDPATH += $$PWD/../../../../../usr/include

#win32: PRE_TARGETDEPS += $$PWD/../../../../../usr/lib/gslcblas.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../../../usr/lib/libgslcblas.a

##

#unix:!symbian|win32: LIBS += -L$$PWD/../../../../../usr/lib/ -lgsl

#INCLUDEPATH += $$PWD/../../../../../usr/include
#DEPENDPATH += $$PWD/../../../../../usr/include

#win32: PRE_TARGETDEPS += $$PWD/../../../../../usr/lib/gsl.lib
#else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../../../usr/lib/libgsl.a

#######################

RESOURCES += \
    res.qrc

########## mathgl ############

unix:!macx: LIBS += -L$$PWD/mathgl/lin64/lib/ -lmgl -lmgl-qt -lmgl-qt5

unix:INCLUDEPATH += $$PWD/mathgl/lin64/include
unix:DEPENDPATH += $$PWD/mathgl/lin64/include

unix:!macx: PRE_TARGETDEPS += $$PWD/mathgl/mathql-2.4.2.1-build.lin64/lib/libmgl.a
unix:!macx: PRE_TARGETDEPS += $$PWD/mathgl/mathql-2.4.2.1-build.lin64/lib/libmgl-qt.a
unix:!macx: PRE_TARGETDEPS += $$PWD/mathgl/mathql-2.4.2.1-build.lin64/lib/libmgl-qt5.a




