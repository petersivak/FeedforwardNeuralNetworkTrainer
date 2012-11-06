#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QStringList>
#include <QListIterator>
#include <QFileDialog>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mNeuralNetworkTrainer(),
    mIsTrainingRunning(false),
    mScene(0),
    mTrainingElapsedTimer(),
    mLastInputSignals(0)
{
    ui->setupUi(this);

    move(QApplication::desktop()->screen()->rect().center()-this->rect().center());   // Center window

    updateVisualizationScene();

    QObject::connect(&mNeuralNetworkTrainer, SIGNAL(finished()), this, SLOT(onNeuralNetworkTrainerFinished()));
    QObject::connect(&mNeuralNetworkTrainer, SIGNAL(percentsDoneValueChanged(int)), ui->trainProgressBar, SLOT(setValue(int)));
}

MainWindow::~MainWindow()
{
    QObject::disconnect(&mNeuralNetworkTrainer, SIGNAL(percentsDoneValueChanged(int)), ui->trainProgressBar, SLOT(setValue(int)));
    QObject::disconnect(&mNeuralNetworkTrainer, SIGNAL(finished()), this, SLOT(onNeuralNetworkTrainerFinished()));

    delete ui;
}

bool MainWindow::validateSamples()
{
    // Get individual samples from samples text edit
    QStringList samples = ui->samplesTextEdit->toPlainText().split("\n");
    int inputSignals = ui->inputSignalsSpinBox->value();

    // Iterate through the samples
    for (QListIterator<QString> it(samples); it.hasNext(); )
    {
        const QString &sample = it.next();

        // First check - sample size must equal inputSignals + 2 (space and output)
        if (sample.size() != inputSignals + 2)
            return false;

        // Second check - first inputSignals characters must be only 0 or 1
        for (int i = 0; i < inputSignals; ++i)
        {
            if ( (sample.at(i) != QChar('0')) && (sample.at(i) != QChar('1')) )
                return false;
        }

        // Third check - there must be space delimiter between inputs and output
        if ( sample.at(inputSignals) != QChar(' ') )
            return false;

        // Fourth check - outputSignal character must be only 0 or 1
        if ( (sample.at(inputSignals + 1) != QChar('0')) && (sample.at(inputSignals + 1) != QChar('1')) )
            return false;
    }

    return true;
}

bool MainWindow::validateTestSample()
{
    // Get a sample from test inputs text edit
    QString sample = ui->testInputsLineEdit->text();

    // First check - sample size must equal inputSignals
    if (sample.size() != mLastInputSignals)
        return false;

    // Second check - characters must be only 0 or 1
    for (int i = 0; i < mLastInputSignals; ++i)
    {
        if ( (sample.at(i) != QChar('0')) && (sample.at(i) != QChar('1')) )
            return false;
    }

    return true;
}

void MainWindow::onTrainStopButtonClicked()
{
    // If train button has been clicked
    if (!mIsTrainingRunning)
    {
        if (!validateSamples())
        {
            ui->elapsedTimeLabel->setText("Samples are not valid!");
            ui->elapsedTimeLabel->setStyleSheet("#elapsedTimeLabel{color:red;}");
            return;
        }

        mIsTrainingRunning = true;

        ui->samplesPanel->setEnabled(false);
        ui->inputsPanel->setEnabled(false);
        ui->resultsPanel->setEnabled(false);
        ui->visualizationPanel->setEnabled(false);
        ui->testPanel->setEnabled(false);
        ui->trainStopButton->setText("Stop Training");
        ui->trainProgressBar->setValue(0);
        ui->trainStackedWidget->setCurrentIndex(0);
        ui->testInputsLineEdit->setText("");
        ui->testOutputLineEdit->setText("");

        mLastInputSignals = ui->inputSignalsSpinBox->value();

        // Do some initializations
        mNeuralNetworkTrainer.setSampleSignals(ui->samplesTextEdit->toPlainText().split("\n"));
        mNeuralNetworkTrainer.setEpochLimit(ui->epochLimitSpinBox->value());
        mNeuralNetworkTrainer.setLearningRate(ui->learningRateSpinBox->value());
        mNeuralNetworkTrainer.setMomentum(ui->momentumSpinBox->value());
        mNeuralNetworkTrainer.setMultithreading(ui->multithreadingCheckBox->isChecked());
        mNeuralNetworkTrainer.setDone(false);

        // Start calculating elapsed time
        mTrainingElapsedTimer.start();

        // Start neural network trainer from different thread
        mNeuralNetworkTrainer.start();
    }
    // If stop training button has been clicked
    else
    {
        mNeuralNetworkTrainer.finish();
    }
}

void MainWindow::onInputSignalsSpinBoxValueChanged(int value)
{
    mNeuralNetworkTrainer.updateWeightsOfFirstLayer(value);

    updateVisualizationScene();
}

void MainWindow::onLayersSpinBoxValueChanged(int value)
{
    // If some layers must be inserted
    if (value > mNeuralNetworkTrainer.numberOfLayers())
    {
        mNeuralNetworkTrainer.insertLayers(value - mNeuralNetworkTrainer.numberOfLayers());
    }
    // If some layers must be deleted
    else if (value < mNeuralNetworkTrainer.numberOfLayers())
    {
        mNeuralNetworkTrainer.deleteLayers(mNeuralNetworkTrainer.numberOfLayers() - value);
    }

    updateVisualizationScene();
}

void MainWindow::updateVisualizationScene()
{
    // Create new scene
    VisualizationGraphicsScene *scene = new VisualizationGraphicsScene(*this);

    // Get const reference to the neural network
    const QList<QList<Neuron> > &neuralNetwork = mNeuralNetworkTrainer.neuralNetworkConstRef();

    // Get number of layers
    int numberOfLayers = neuralNetwork.size();

    // Get number of elements of the "largest" layers (input signals included)
    int largestLayerElements = ui->inputSignalsSpinBox->value();
    for (int i = 0; i < neuralNetwork.size(); ++i)
    {
        if (neuralNetwork[i].size() > largestLayerElements)
        {
            largestLayerElements = neuralNetwork[i].size();
        }
    }

    // Add neuron circles
    for (int i = 0; i < neuralNetwork.size(); ++i)
    {
        for (int j = 0; j < neuralNetwork[i].size(); ++j)
        {
            int neuronShift = ((largestLayerElements * 50) - (neuralNetwork[i].size() * 50)) / 2;
            scene->addEllipse(40 + i * 80, -20 + j * 50 + neuronShift, 40, 40, QPen(), QBrush(Qt::green));
        }
    }

    // Add lines from input signals to neurons in the first layer
    for (int i = 0; i < ui->inputSignalsSpinBox->value(); ++i)
    {
        for (int j = 0; j < neuralNetwork[0].size(); ++j)
        {
            int inputSignalsShift = ((largestLayerElements * 50) - (ui->inputSignalsSpinBox->value() * 50)) / 2;
            int firstLayerShift = ((largestLayerElements * 50) - (neuralNetwork[0].size() * 50)) / 2;
            scene->addLine(0, i * 50 + inputSignalsShift, 40, j * 50 + firstLayerShift, QPen(Qt::black, 1));
        }
    }

    // Add lines in hidden layers
    for (int i = 0; i < neuralNetwork.size() - 1; ++i)
    {
        for (int j = 0; j < neuralNetwork[i].size(); ++j)
        {
            for (int k = 0; k < neuralNetwork[i + 1].size(); ++k)
            {
                int lineStartShift = ((largestLayerElements * 50) - (neuralNetwork[i].size() * 50)) / 2;
                int lineEndShift = ((largestLayerElements * 50) - (neuralNetwork[i + 1].size() * 50)) / 2;
                scene->addLine(80 + i * 80, j * 50 + lineStartShift, 80 + i * 80 + 40, k * 50 + lineEndShift, QPen(Qt::black, 1));
            }
        }
    }

    // Add line from neuron in the last layer to output signal
    int lastLayerShift = ((largestLayerElements * 50) - (1 * 50)) / 2;
    scene->addLine(numberOfLayers * 80, lastLayerShift, numberOfLayers * 80 + 40, lastLayerShift, QPen(Qt::black, 1));

    // Show scene on the visualization graphics view
    ui->visualizationGraphicsView->setScene(scene);

    // Delete old scene and point to new scene
    delete mScene;
    mScene = scene;
}

void MainWindow::insertNeuronToLayer(int layer)
{
    // Do nothing for "unwanted" layers
    if (layer < 0 || layer >= mNeuralNetworkTrainer.numberOfLayers() - 1)
        return;

    mNeuralNetworkTrainer.insertNeuron(layer, ui->inputSignalsSpinBox->value());

    updateVisualizationScene();
}

void MainWindow::deleteNeuronFromLayer(int layer)
{
    // Do nothing for "unwanted" layers
    if (layer < 0 || layer >= mNeuralNetworkTrainer.numberOfLayers() - 1)
        return;

    mNeuralNetworkTrainer.deleteNeuron(layer, ui->inputSignalsSpinBox->value());

    updateVisualizationScene();
}

void MainWindow::onNeuralNetworkTrainerFinished()
{
    int elapsedTime = mTrainingElapsedTimer.elapsed() / 1000;
    mIsTrainingRunning = false;

    ui->samplesPanel->setEnabled(true);
    ui->inputsPanel->setEnabled(true);
    ui->visualizationPanel->setEnabled(true);
    ui->testPanel->setEnabled(true);
    ui->trainStopButton->setText("Train The Network");
    ui->elapsedTimeLabel->setText(QString("Elapsed Time: ") + QString::number(elapsedTime) + QString("s"));
    ui->elapsedTimeLabel->setStyleSheet("");
    ui->trainStackedWidget->setCurrentIndex(1);

    // Fill the results text edit
    ui->resultsPanel->setEnabled(true);
    ui->resultsTextEdit->setPlainText("");
    const QList<double> &successRates = mNeuralNetworkTrainer.successRatesConstRef();
    const QList<double> &meanSquareErrors = mNeuralNetworkTrainer.meanSquareErrorsConstRef();
    for (int i = 0; i < successRates.size(); ++i)
    {
        ui->resultsTextEdit->appendPlainText(QString::number(i) + QString(" ") + QString::number(successRates[i], 'g', 2) +
                                             QString(" ") + QString::number(meanSquareErrors[i], 'g', 2));
    }
}

void MainWindow::onTestButtonClicked()
{
    if (!validateTestSample())
    {
        ui->elapsedTimeLabel->setText("Test sample is not valid!");
        ui->elapsedTimeLabel->setStyleSheet("#elapsedTimeLabel{color:red;}");
        return;
    }

    if (ui->elapsedTimeLabel->text() == "Test sample is not valid!")
    {
        ui->elapsedTimeLabel->setText("");
        ui->elapsedTimeLabel->setStyleSheet("");
    }
    QString outputSignal = mNeuralNetworkTrainer.calculateOutputSignal(ui->testInputsLineEdit->text());
    ui->testOutputLineEdit->setText(outputSignal);
}

void MainWindow::onLoadFromFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File With Samples"), ".",
                                            tr("Files (*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString fileContent = in.readAll();
    ui->samplesTextEdit->setPlainText(fileContent.remove(fileContent.length() - 1, 1));
}
