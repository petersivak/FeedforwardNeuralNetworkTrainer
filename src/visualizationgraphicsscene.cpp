#include "include/visualizationgraphicsscene.hpp"

#include <QGraphicsSceneMouseEvent>

#include "mainwindow.hpp"

VisualizationGraphicsScene::VisualizationGraphicsScene(MainWindow &mainWindowConstRef) :
    QGraphicsScene(0),
    mMainWindowConstRef(mainWindowConstRef)
{
}

void VisualizationGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    int pressedLayer = -1;

    // With this dirty computation :) we can find out, which layer was pressed
    if ( ((((int) event->scenePos().x() - 40) / 80) == (((int) event->scenePos().x() - 40 + 40) / 80)) &&
         (event->scenePos().x() >= 40) )
        pressedLayer = (((int) event->scenePos().x() - 40) / 80);

    if (event->button() == Qt::LeftButton)
        mMainWindowConstRef.insertNeuronToLayer(pressedLayer);
    else if (event->button() == Qt::RightButton)
        mMainWindowConstRef.deleteNeuronFromLayer(pressedLayer);
}

void VisualizationGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    int pressedLayer = -1;

    // With this dirty computation :) we can find out, which layer was pressed
    if ( ((((int) event->scenePos().x() - 40) / 80) == (((int) event->scenePos().x() - 40 + 40) / 80)) &&
         (event->scenePos().x() >= 40) )
        pressedLayer = (((int) event->scenePos().x() - 40) / 80);

    if (event->button() == Qt::LeftButton)
        mMainWindowConstRef.insertNeuronToLayer(pressedLayer);
    else if (event->button() == Qt::RightButton)
        mMainWindowConstRef.deleteNeuronFromLayer(pressedLayer);
}
