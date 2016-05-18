#-------------------------------------------------
#
# Project created by QtCreator 2013-09-07T19:43:51
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pricewo
TEMPLATE = app

DESTDIR = bin

SOURCES += main.cpp\
        mainwindow.cpp \
    dlg_result.cpp \
    dlg_settings.cpp \
    adddialog.cpp \
    dlg_addtoorder.cpp \
    dlg_order.cpp \
    dlg_saveorder.cpp \
    dlg_email.cpp \
    dlg_composeletter.cpp \
    files.cpp \
    threads.cpp

HEADERS  += mainwindow.h \
    dlg_result.h \
    dlg_settings.h \
    adddialog.h \
    dlg_addtoorder.h \
    dlg_order.h \
    dlg_saveorder.h \
    dlg_email.h \
    dlg_composeletter.h \
    files.h \
    threads.h

FORMS    += mainwindow.ui \
    dlg_result.ui \
    dlg_settings.ui \
    adddialog.ui \
    dlg_addtoorder.ui \
    dlg_order.ui \
    dlg_saveorder.ui \
    dlg_email.ui \
    dlg_composeletter.ui

unix: LIBS += -Llib/freexl/ -lfreexl
INCLUDEPATH += lib/include/freexl
DEPENDPATH += lib/include/freexl
unix: PRE_TARGETDEPS += lib/freexl/libfreexl.a

unix: LIBS += -Llib/xlslib/ -lxls
INCLUDEPATH += lib/include/xlslib
DEPENDPATH += lib/include/xlslib
unix: PRE_TARGETDEPS += lib/xlslib/libxls.a

include(qtxlsx/qtxlsx.pri)

include(smtpclient/SMTPEmail.pri)
