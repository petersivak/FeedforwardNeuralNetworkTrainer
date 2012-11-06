#include "include/neuralnetworktrainer.hpp"

#include <cmath>
#include <QStringList>
#include <QListIterator>
#include <QDebug>

#include "utilities.hpp"

NeuralNetworkTrainer::NeuralNetworkTrainer() :
    mDone(false),
    mSampleInputSignals(),
    mSampleOutputSignals(),
    mNeuralNetwork(),
    mEpochLimit(0),
    mLearningRate(0.0),
    mMomentum(0.0),
    mMultithreading(false),
    mSuccessRates(),
    mMeanSquareErrors()
{
    // Add an initial neuron to the neural network
    QList<Neuron> neurons;
    neurons.append(Neuron());
    mNeuralNetwork.append(neurons);
}

void NeuralNetworkTrainer::finish()
{
    mDone = true;
    wait();
}

void NeuralNetworkTrainer::setSampleSignals(const QStringList &samples)
{
    // Clear previous content of sample signals
    mSampleInputSignals.clear();
    mSampleOutputSignals.clear();

    // Fill sample input and output signals
    for (QListIterator<QString> it(samples); it.hasNext(); )
    {
        const QString &sample = it.next();

        // Fill sample input signals
        QList<char> sampleInputSignals;
        for (int i = 0; i < sample.size() - 2; ++i)
        {
            sampleInputSignals.append(sample.at(i).toAscii() - 48);
        }
        mSampleInputSignals.append(sampleInputSignals);

        // Fill sample output signal
        mSampleOutputSignals.append(sample.at(sample.size() - 1).toAscii() - 48);
    }
}

int NeuralNetworkTrainer::numberOfLayers() const
{
    return mNeuralNetwork.size();
}

void NeuralNetworkTrainer::insertLayers(int number)
{
    for (int i = 0; i < number; ++i)
    {
        QList<Neuron> neurons;
        neurons.append(Neuron());
        mNeuralNetwork.append(neurons);
    }
}

void NeuralNetworkTrainer::deleteLayers(int number)
{
    for (int i = 0; i < number; ++i)
    {
        mNeuralNetwork.removeLast();
    }

    // If the last layer contains now more than 1 element, delete all elements
    // but the one
    if (mNeuralNetwork[numberOfLayers() - 1].size() > 1)
    {
        int tempSize = mNeuralNetwork[numberOfLayers() - 1].size() - 1;
        for (int i = 0; i < tempSize; ++i)
        {
            mNeuralNetwork[numberOfLayers() - 1].removeLast();
        }
    }
}

const QList<QList<Neuron> > &NeuralNetworkTrainer::neuralNetworkConstRef() const { return mNeuralNetwork; }

void NeuralNetworkTrainer::insertNeuron(int layer, int intputSignals)
{
    Neuron neuron;

    // If the neuron has been inserted into the first layer
    if (layer == 0)
    {
        for (int i = 0; i < intputSignals - 1; ++i)
        {
            neuron.insertWeight();
        }
    }
    // If the neuron has been inserted into another layer
    else
    {
        for (int i = 0; i < mNeuralNetwork[layer - 1].size() - 1; ++i)
        {
            neuron.insertWeight();
        }
    }

    for (int i = 0; i < mNeuralNetwork[layer + 1].size(); ++i)
    {
        mNeuralNetwork[layer + 1][i].insertWeight();
    }

    mNeuralNetwork[layer].append(neuron);
}

void NeuralNetworkTrainer::deleteNeuron(int layer, int intputSignals)
{
    if (mNeuralNetwork[layer].size() >= 2)
    {
        mNeuralNetwork[layer].removeLast();

        for (int i = 0; i < mNeuralNetwork[layer + 1].size(); ++i)
        {
            mNeuralNetwork[layer + 1][i].deleteWeight();
        }
    }
}

void NeuralNetworkTrainer::setEpochLimit(int limit) { mEpochLimit = limit; }
void NeuralNetworkTrainer::setLearningRate(double learningRate) { mLearningRate = learningRate; }
void NeuralNetworkTrainer::setMomentum(double momentum) { mMomentum = momentum; }
void NeuralNetworkTrainer::setMultithreading(bool multithreading) { mMultithreading = multithreading; }

const QList<double> &NeuralNetworkTrainer::successRatesConstRef() const { return mSuccessRates;}
const QList<double> &NeuralNetworkTrainer::meanSquareErrorsConstRef() const { return mMeanSquareErrors; }

void NeuralNetworkTrainer::updateWeightsOfFirstLayer(int inputSignals)
{
    for (int i = 0; i < mNeuralNetwork[0].size(); ++i)
    {
        if (inputSignals > mNeuralNetwork[0][i].numberOfWeights())
        {
            int tempSize = mNeuralNetwork[0][i].numberOfWeights();
            for (int j = 0; j < inputSignals - tempSize; ++j)
            {
                mNeuralNetwork[0][i].insertWeight();
            }
        }
        else if (inputSignals < mNeuralNetwork[0][i].numberOfWeights())
        {
            int tempSize = mNeuralNetwork[0][i].numberOfWeights();
            for (int j = 0; j < tempSize - inputSignals; ++j)
            {
                mNeuralNetwork[0][i].deleteWeight();
            }
        }
    }
}

void NeuralNetworkTrainer::setDone(bool value) { mDone = value; }

QString NeuralNetworkTrainer::calculateOutputSignal(const QString &sample)
{
    // Fill sample input signals
    QList<char> sampleInputSignals;
    for (int i = 0; i < sample.size(); ++i)
    {
        sampleInputSignals.append(sample.at(i).toAscii() - 48);
    }

    // Calculate output signals of all neurons
    for (int j = 0; j < mNeuralNetwork.size(); ++j)
    {
        for (int k = 0; k < mNeuralNetwork[j].size(); ++k)
        {
            Neuron &currentNeuron = mNeuralNetwork[j][k];

            double sum = 0;

            // If this is the first layer
            if (j == 0)
            {
                for (int l = 0; l < sampleInputSignals.size(); ++l)
                {
                    sum += currentNeuron.weight(l) * sampleInputSignals[l];
                }
            }
            // If this is another layer
            else
            {
                for (int l = 0; l < mNeuralNetwork[j - 1].size(); ++l)
                {
                    sum += currentNeuron.weight(l) * mNeuralNetwork[j - 1][l].outputSignal();
                }
            }

            // Apply sigmoid function to the sum and store the result as the neuron output signal
            currentNeuron.setSumOfProductsOfWeightsAndInputSignals(sum);
            currentNeuron.setOutputSignal(sigmoidFunction(sum - currentNeuron.threshold()));
        }
    }

    double realOutputSignal = mNeuralNetwork[mNeuralNetwork.size() - 1][0].outputSignal();
    QString outputSignalString = "?";   // Question mark if undefined

    if (realOutputSignal <= 0.4)
        outputSignalString = "0";
    else if (realOutputSignal >= 0.6)
        outputSignalString = "1";

    return outputSignalString;
}

void NeuralNetworkTrainer::run()
{
    /*double z = 4;
    for (int i = 0; i < 100000000; ++i)
    {
        z *= pow(pow(pow(pow(pow(pow(235.35, 235.53), 5000), 345.345), 636.654), 45.6654), 346.653);
    }
    return;*/

    // Do some initializations
    int arrayWithSamplesOrder[mSampleInputSignals.size()];
    for (int i = 0; i < mSampleInputSignals.size(); ++i)
    {
        arrayWithSamplesOrder[i] = i;
    }
    int epoch = 0;
    int percentsDone = 0;
    mSuccessRates.clear();
    mMeanSquareErrors.clear();

    // Randomly initialize weights and threshold of each neuron
    for (int i = 0; i < mNeuralNetwork.size(); ++i)
    {
        for (int j = 0; j < mNeuralNetwork[i].size(); ++j)
        {
            // If this is first layer
            if (i == 0)
            {
                for (int k = 0; k < mSampleInputSignals[0].size(); ++k)
                {
                    mNeuralNetwork[i][j].setWeight(k, (double) ((realRandom() % 100) - 50) / 100);
                }
            }
            // If this is another layer
            else
            {
                for (int k = 0; k < mNeuralNetwork[i - 1].size(); ++k)
                {
                    mNeuralNetwork[i][j].setWeight(k, (double) ((realRandom() % 100) - 50) / 100);
                }
            }

            mNeuralNetwork[i][j].setThreshold((double) ((realRandom() % 100) - 50) / 100);
        }
    }

    // Train the network until done
    while (!mDone)
    {
        // We are moving into the next epoch
        ++epoch;
        int newPercentsDone = (int) (((double) epoch / mEpochLimit) * 100);
        if (newPercentsDone > percentsDone)
        {
            percentsDone = newPercentsDone;
            emit percentsDoneValueChanged(percentsDone);
        }

        // Shuffle samples order
        shuffle(arrayWithSamplesOrder, mSampleInputSignals.size());

        // For each sample
        for (int i = 0; i < mSampleInputSignals.size(); ++i)
        {
            // Calculate output signals of all neurons
            for (int j = 0; j < mNeuralNetwork.size(); ++j)
            {
                for (int k = 0; k < mNeuralNetwork[j].size(); ++k)
                {
                    Neuron &currentNeuron = mNeuralNetwork[j][k];

                    double sum = 0;

                    // If this is the first layer
                    if (j == 0)
                    {
                        for (int l = 0; l < mSampleInputSignals[i].size(); ++l)
                        {
                            sum += currentNeuron.weight(l) * mSampleInputSignals[i][l];
                        }
                    }
                    // If this is another layer
                    else
                    {
                        for (int l = 0; l < mNeuralNetwork[j - 1].size(); ++l)
                        {
                            sum += currentNeuron.weight(l) * mNeuralNetwork[j - 1][l].outputSignal();
                        }
                    }

                    // Apply sigmoid function to the sum and store the result as the neuron output signal
                    currentNeuron.setSumOfProductsOfWeightsAndInputSignals(sum);
                    currentNeuron.setOutputSignal(sigmoidFunction(sum - currentNeuron.threshold()));
                }
            }

            // Calculate error signal of the last neuron
            Neuron &lastNeuron = mNeuralNetwork[mNeuralNetwork.size() - 1][0];
            lastNeuron.setErrorSignal(mSampleOutputSignals[i] - lastNeuron.outputSignal());

            // Calculate error signals of all other neurons
            for (int j = mNeuralNetwork.size() - 2; j >= 0; --j)
            {
                for (int k = 0; k < mNeuralNetwork[j].size(); ++k)
                {
                    Neuron &currentNeuron = mNeuralNetwork[j][k];

                    double sum = 0;

                    for (int l = 0; l < mNeuralNetwork[j + 1].size(); ++l)
                    {
                        sum += mNeuralNetwork[j + 1][l].weight(k) * mNeuralNetwork[j + 1][l].errorSignal();
                    }

                    // Store the result as the neuron error signal
                    currentNeuron.setErrorSignal(sum);
                }
            }

            // Modify weights and threshold of all neurons
            for (int j = 0; j < mNeuralNetwork.size(); ++j)
            {
                for (int k = 0; k < mNeuralNetwork[j].size(); ++k)
                {
                    Neuron &currentNeuron = mNeuralNetwork[j][k];

                    // If this is the first layer
                    if (j == 0)
                    {
                        for (int l = 0; l < currentNeuron.numberOfWeights(); ++l)
                        {
                            double weightDelta = mLearningRate * currentNeuron.errorSignal() *
                                    sigmoidFunction(currentNeuron.sumOfProductsOfWeightsAndInputSignals() - currentNeuron.threshold()) *
                                    (1 - sigmoidFunction(currentNeuron.sumOfProductsOfWeightsAndInputSignals() - currentNeuron.threshold())) *
                                    mSampleInputSignals[i][l];
                            currentNeuron.setWeight(l, currentNeuron.weight(l) + weightDelta + mMomentum *
                                                    currentNeuron.lastWeightDelta(l));
                            currentNeuron.setLastWeightDelta(l, weightDelta);
                        }
                    }
                    // If this is another layer
                    else
                    {
                        for (int l = 0; l < currentNeuron.numberOfWeights(); ++l)
                        {
                            double weightDelta = mLearningRate * currentNeuron.errorSignal() *
                                    sigmoidFunction(currentNeuron.sumOfProductsOfWeightsAndInputSignals() - currentNeuron.threshold()) *
                                    (1 - sigmoidFunction(currentNeuron.sumOfProductsOfWeightsAndInputSignals() - currentNeuron.threshold())) *
                                    mNeuralNetwork[j - 1][l].outputSignal();
                            currentNeuron.setWeight(l, currentNeuron.weight(l) + weightDelta + mMomentum *
                                                    currentNeuron.lastWeightDelta(l));
                            currentNeuron.setLastWeightDelta(l, weightDelta);
                        }
                    }

                    double thresholdDelta = mLearningRate * currentNeuron.errorSignal() *
                            sigmoidFunction(currentNeuron.sumOfProductsOfWeightsAndInputSignals() - currentNeuron.threshold()) *
                            (1 - sigmoidFunction(currentNeuron.sumOfProductsOfWeightsAndInputSignals() - currentNeuron.threshold())) * -1;
                    currentNeuron.setThreshold(currentNeuron.threshold() + thresholdDelta +
                                               mMomentum * currentNeuron.lastThresholdDelta());
                    currentNeuron.setLastThresholdDelta(thresholdDelta);
                }
            }
        }

        // Calculate the success rate and mean square error (again iterate through each sample)
        int numberOfSuccessfullyClassifiedSamples = 0;
        double meanSquareError = 0;
        for (int i = 0; i < mSampleInputSignals.size(); ++i)
        {
            // Calculate output signals of all neurons
            for (int j = 0; j < mNeuralNetwork.size(); ++j)
            {
                for (int k = 0; k < mNeuralNetwork[j].size(); ++k)
                {
                    Neuron &currentNeuron = mNeuralNetwork[j][k];

                    double sum = 0;

                    // If this is the first layer
                    if (j == 0)
                    {
                        for (int l = 0; l < mSampleInputSignals[i].size(); ++l)
                        {
                            sum += currentNeuron.weight(l) * mSampleInputSignals[i][l];
                        }
                    }
                    // If this is another layer
                    else
                    {
                        for (int l = 0; l < mNeuralNetwork[j - 1].size(); ++l)
                        {
                            sum += currentNeuron.weight(l) * mNeuralNetwork[j - 1][l].outputSignal();
                        }
                    }

                    // Apply sigmoid function to the sum and store the result as the neuron output signal
                    currentNeuron.setSumOfProductsOfWeightsAndInputSignals(sum);
                    currentNeuron.setOutputSignal(sigmoidFunction(sum - currentNeuron.threshold()));
                }
            }

            double realOutputSignal = mNeuralNetwork[mNeuralNetwork.size() - 1][0].outputSignal();

            if (realOutputSignal <= 0.4)
                realOutputSignal = 0;
            else if (realOutputSignal >= 0.6)
                realOutputSignal = 1;
            else
                realOutputSignal = 1000;   // Something "undefined"

            if ( (char) realOutputSignal == mSampleOutputSignals[i] )
                ++numberOfSuccessfullyClassifiedSamples;

            //qDebug() << mSampleOutputSignals[i] - mNeuralNetwork[mNeuralNetwork.size() - 1][0].outputSignal();
            meanSquareError += pow(mSampleOutputSignals[i] - mNeuralNetwork[mNeuralNetwork.size() - 1][0].outputSignal(), 2);
        }
        double successRate = (double) numberOfSuccessfullyClassifiedSamples / (double) mSampleOutputSignals.size();
        meanSquareError /= mSampleOutputSignals.size();
        mSuccessRates.append(successRate);
        mMeanSquareErrors.append(meanSquareError);
        // If success rate is 100% or epoch limit has been reached, stop the training process
        if ((char) successRate == 1 || epoch >= mEpochLimit)
        {
            mDone = true;
            continue;
        }
    }
}

double NeuralNetworkTrainer::sigmoidFunction(double input)
{
    return (1 / (1 + exp(-input)));
}
