#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP

#include <QThread>
#include <QObject>
#include <QList>

#include "neuron.hpp"

class NeuralNetworkTrainer : public QThread
{
    Q_OBJECT
public:
    NeuralNetworkTrainer();

    /**
     * @brief This method is meant to be called from a thread, which wants to
     * finish the execution of this thread.
     */
    void finish();

    /**
     * @brief Fill sample input and output signals.
     * @param samples input and output signals
     */
    void setSampleSignals(const QStringList &samples);

    /**
     * @brief Get number of layers of the neural network.
     * @return number of layers
     */
    int numberOfLayers() const;

    /**
     * @brief Inserts number of new layers.
     * @param number number of layers to be inserted
     */
    void insertLayers(int number);

    /**
     * @brief Delete number of last layers.
     * @param number number of last layers to be deleted
     */
    void deleteLayers(int number);

    /**
     * @brief Gets const reference to the neural network.
     * @return const reference to the neural network
     */
    const QList< QList<Neuron> > &neuralNetworkConstRef() const;

    /**
     * @brief Inserts new neuron to specified layer.
     * @param layer layer where new neuron is to be inserted
     */
    void insertNeuron(int layer, int intputSignals);

    /**
     * @brief Deletes last neuron from this layer (only if there are 2 or more
     * neurons.
     * @param layer layer where last neuron is to be deleted
     */
    void deleteNeuron(int layer, int intputSignals);

    /**
     * @brief Sets epoch limit.
     * @param limit the epoch limit
     */
    void setEpochLimit(int limit);

    /**
     * @brief Sets learning rate.
     * @param learningRate learning rate
     */
    void setLearningRate(double learningRate);

    /**
     * @brief Sets momentum.
     * @param momentum momentum
     */
    void setMomentum(double momentum);

    /**
     * @brief Sets epoch limit.
     * @param limit the epoch limit
     */
    void setMultithreading(bool multithreading);

    /**
     * @brief Gets const reference to success rates.
     * @return const reference to success rates
     */
    const QList<double> &successRatesConstRef() const;

    /**
     * @brief Gets const reference to mean square errors
     * @return const reference to mean square errors
     */
    const QList<double> &meanSquareErrorsConstRef() const;

    /**
     * @brief Updates weights of first layer.
     */
    void updateWeightsOfFirstLayer(int inputSignals);

    void setDone(bool value);

    QString calculateOutputSignal(const QString &sample);

private:
    void run();

    double sigmoidFunction(double input);
    
signals:
    void percentsDoneValueChanged(int value);
    
public slots:

private:
    bool mDone;

    QList< QList<char> > mSampleInputSignals;
    QList<char> mSampleOutputSignals;

    QList< QList<Neuron> > mNeuralNetwork;

    int mEpochLimit;
    double mLearningRate;
    double mMomentum;
    bool mMultithreading;

    QList<double> mSuccessRates;
    QList<double> mMeanSquareErrors;
};

#endif // NEURALNETWORK_HPP
