QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = NerveTractsLab
LIBS += -lopengl32

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ROutlinerData.cpp \
    alignmesh.cpp \
    allignwidget.cpp \
    allignwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    meshcutter.cpp \
    meshmodel.cpp \
    meshmodelloader.cpp \
    nrtlmanager.cpp \
    nrtlmodel.cpp \
    oglareawidget.cpp \
    outliner.cpp \
    outlinerwidget.cpp \
    renamestepdialog.cpp \
    resourcemanager.cpp \
    rgroupid.cpp \
    rmeshmodel.cpp \
    rsectionmodel.cpp \
    rstep.cpp \
    transparencydialog.cpp

HEADERS += \
    MeshAlgorithm.h \
    NrtlType.h \
    allignwidget.h \
    allignwindow.h \
    ivcontainer.h \
    mainwindow.h  \
    meshmodel.h \
    nrtlmanager.h \
    nrtlmodel.h \
    oglareawidget.h \
    outliner.h \
    outlinerwidget.h \
    renamestepdialog.h \
    resourcemanager.h \
    transparencydialog.h

FORMS += \
    allignwindow.ui \
    dialog.ui \
    mainwindow.ui \
    transparencydialog.ui

RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
