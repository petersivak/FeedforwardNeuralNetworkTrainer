#-------------------------------------------------
#
# Project created by QtCreator 2012-10-24T22:30:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FeedforwardNeuralNetworkTrainer
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/neuron.cpp \
    src/utilities.cpp \
    src/neuralnetworktrainer.cpp \
    src/visualizationgraphicsscene.cpp

HEADERS  += include/mainwindow.hpp \
    include/neuron.hpp \
    include/utilities.hpp \
    include/neuralnetworktrainer.hpp \
    include/visualizationgraphicsscene.hpp

FORMS    += forms/mainwindow.ui

INCLUDEPATH += include
DEPENDPATH += include
