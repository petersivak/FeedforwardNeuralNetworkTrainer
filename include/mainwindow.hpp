#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <QGraphicsScene>
#include <QElapsedTimer>

#include "neuralnetworktrainer.hpp"
#include "visualizationgraphicsscene.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * @brief This method checks several conditions which must be verified
     * to samples be valid.
     * @return whether samples are valid or not
     */
    bool validateSamples();

    bool validateTestSample();

public slots:
    void onTrainStopButtonClicked();
    void onInputSignalsSpinBoxValueChanged(int value);
    void onLayersSpinBoxValueChanged(int value);
    void updateVisualizationScene();
    void insertNeuronToLayer(int layer);
    void deleteNeuronFromLayer(int layer);
    void onNeuralNetworkTrainerFinished();
    void onTestButtonClicked();
    void onLoadFromFileButtonClicked();
    
private:
    Ui::MainWindow *ui;

    NeuralNetworkTrainer mNeuralNetworkTrainer;

    bool mIsTrainingRunning;

    VisualizationGraphicsScene *mScene;

    QElapsedTimer mTrainingElapsedTimer;

    int mLastInputSignals;
};

#endif // MAINWINDOW_HPP
