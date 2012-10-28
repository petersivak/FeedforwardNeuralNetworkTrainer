#ifndef NEURON_HPP
#define NEURON_HPP

class Neuron
{
public:
    Neuron();

private:
    double mWeight;
    double mOutput;
    double mErrorSignal;
};

#endif // NEURON_HPP
