#ifndef NEURON_HPP
#define NEURON_HPP

#include <QList>

class Neuron
{
public:
    Neuron();

    double weight(int i) const;
    double threshold() const;
    double outputSignal() const;
    double errorSignal() const;
    double sumOfProductsOfWeightsAndInputSignals() const;
    double lastWeightDelta(int i) const;
    double lastThresholdDelta() const;

    void setWeight(int i, double weight);
    void setThreshold(double threshold);
    void setOutputSignal(double signal);
    void setErrorSignal(double signal);
    void setSumOfProductsOfWeightsAndInputSignals(double sum);
    void setLastWeightDelta(int i, double delta);
    void setLastThresholdDelta(double delta);

    int numberOfWeights() const;

    void insertWeight();
    void deleteWeight();

private:
    QList<double> mWeights;
    double mThreshold;
    double mOutputSignal;
    double mErrorSignal;
    double mSumOfProductsOfWeightsAndInputSignals;
    QList<double> mLastWeightDeltas;
    double mLastThresholdDelta;
};

#endif // NEURON_HPP
