#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP

#include <QObject>

#include <QList>

class NeuralNetwork : public QObject
{
    Q_OBJECT
public:
    explicit NeuralNetwork(QObject *parent = 0);
    
signals:
    
public slots:

private:
    QList<char> mSample;
};

#endif // NEURALNETWORK_HPP
