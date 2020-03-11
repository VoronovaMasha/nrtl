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

WGTPATH = ./widgets
COREPATH = ./core
OGLPATH = ./opengl

MODELPATH = $$COREPATH/model
MNGRPATH = $$COREPATH/manager
ALGPATH = $$COREPATH/meshalgorithm

INCLUDEPATH += $$WGTPATH $$COREPATH $$OGLPATH $$MODELPATH $$MNGRPATH $$ALGPATH

SOURCES += \
    core/manager/ROutlinerData.cpp \
    core/manager/RTractM.cpp \
    core/manager/nrtlmanager.cpp \
    core/manager/projectoptions.cpp \
    core/manager/rgroupid.cpp \
    core/manager/rmeshmodel.cpp \
    core/manager/rsectionmodel.cpp \
    core/manager/rstep.cpp \
    core/meshalgorithm/alignmesh.cpp \
    core/meshalgorithm/meshcutter.cpp \
    core/meshalgorithm/mesheraser.cpp \
    core/meshalgorithm/meshmodelloader.cpp \
    core/meshalgorithm/meshtopology.cpp \
    core/model/meshmodel.cpp \
    core/model/nrtlmodel.cpp \
    core/model/outliner.cpp \
    main.cpp \
    opengl/allignwidget.cpp \
    opengl/oglareawidget.cpp \
    $$WGTPATH/groupeditorwidget.cpp \
    $$WGTPATH/setcolor.cpp \
    $$WGTPATH/tractwidget.cpp \
    widgets/allignwindow.cpp \
    widgets/dialog1.cpp \
    widgets/dialog2.cpp \
    widgets/mainwindow.cpp \
    widgets/outlinerwidget.cpp \
    widgets/renamestepdialog.cpp \
    widgets/resourcemanager.cpp \
    widgets/transparencydialog.cpp

HEADERS += \
    core/NrtlType.h \
    core/manager/nrtlmanager.h \
    core/meshalgorithm/MeshAlgorithm.h \
    core/model/ivcontainer.h \
    core/model/meshmodel.h \
    core/model/nrtlmodel.h \
    core/model/outliner.h \
    opengl/allignwidget.h \
    opengl/oglareawidget.h \
    $$WGTPATH/groupeditorwidget.h \
    $$WGTPATH/setcolor.h \
    $$WGTPATH/tractwidget.h \
    widgets/allignwindow.h \
    widgets/dialog1.h \
    widgets/dialog2.h \
    widgets/mainwindow.h \
    widgets/outlinerwidget.h \
    widgets/renamestepdialog.h \
    widgets/resourcemanager.h \
    widgets/transparencydialog.h \
    widgets/widgettype.h

FORMS += \
    $$WGTPATH/setcolor.ui \
    ui/allignwindow.ui \
    ui/dialog.ui \
    ui/mainwindow.ui \
    ui/transparencydialog.ui \
    widgets/dialog1.ui \
    widgets/dialog2.ui

RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
