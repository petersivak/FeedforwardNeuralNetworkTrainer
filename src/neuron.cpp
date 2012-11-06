#include "include/neuron.hpp"

Neuron::Neuron() :
    mWeights(),
    mThreshold(0.0),
    mOutputSignal(0.0),
    mErrorSignal(0.0),
    mSumOfProductsOfWeightsAndInputSignals(0.0),
    mLastWeightDeltas(),
    mLastThresholdDelta(0.0)
{
    // Every created neuron has just one input - thus one weight (and one weight delta)
    mWeights.append(0.0);
    mLastWeightDeltas.append(0.0);
}

double Neuron::threshold() const { return mThreshold; }
double Neuron::weight(int i) const { return mWeights[i]; }
double Neuron::outputSignal() const { return mOutputSignal; }
double Neuron::errorSignal() const { return mErrorSignal; }
double Neuron::sumOfProductsOfWeightsAndInputSignals() const { return mSumOfProductsOfWeightsAndInputSignals; }
double Neuron::lastWeightDelta(int i) const { return mLastWeightDeltas[i]; }
double Neuron::lastThresholdDelta() const { return mLastThresholdDelta; }

void Neuron::setThreshold(double threshold) { mThreshold = threshold; }
void Neuron::setWeight(int i, double weight) { mWeights[i] = weight; }
void Neuron::setOutputSignal(double signal) { mOutputSignal = signal; }
void Neuron::setErrorSignal(double signal) { mErrorSignal = signal; }
void Neuron::setSumOfProductsOfWeightsAndInputSignals(double sum) { mSumOfProductsOfWeightsAndInputSignals = sum; }
void Neuron::setLastWeightDelta(int i, double delta) { mLastWeightDeltas[i] = delta; }
void Neuron::setLastThresholdDelta(double delta) { mLastThresholdDelta = delta; }

int Neuron::numberOfWeights() const
{
    return mWeights.size();
}

void Neuron::insertWeight()
{
    mWeights.append(0.0);
    mLastWeightDeltas.append(0.0);
}

void Neuron::deleteWeight()
{
    mWeights.removeLast();
    mLastWeightDeltas.removeLast();
}
