QT       += core gui multimedia
win32:LIBS += -luser32
win32:LIBS += -liphlpapi

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

win32 {
CONFIG += embed_manifest_exe
QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
RC_ICONS = icon.ico
}

SOURCES += \
    binding_dialog.cpp \
    key_listener.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    binding_dialog.h \
    key_listener.h \
    mainwindow.h

FORMS += \
    binding_dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    peko.jpg

RESOURCES += \
    res.qrc
