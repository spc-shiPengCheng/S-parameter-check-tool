QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QT += printsupport

DEFINES += QCUSTOMPLOT_COMPILE_LIBRARY

INCLUDEPATH += ./eigen-5.0.0

QMAKE_CXXFLAGS += -Wa,-mbig-obj

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    st_file_io.cpp \
    check_sparameter.cpp \
    qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    st_file_io.h \
    st_struct_file.h \
    check_sparameter.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    SParameterCheckTool_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resource.qrc
