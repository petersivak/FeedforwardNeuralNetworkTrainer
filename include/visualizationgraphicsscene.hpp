#ifndef VISUALIZATIONGRAPHICSSCENE_HPP
#define VISUALIZATIONGRAPHICSSCENE_HPP

#include <QGraphicsScene>

class MainWindow;   // Forward declaration

class VisualizationGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    VisualizationGraphicsScene(MainWindow &mainWindowConstRef);
    
signals:
    
public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    
private:
    MainWindow &mMainWindowConstRef;
};

#endif // VISUALIZATIONGRAPHICSSCENE_HPP
