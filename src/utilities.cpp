#include "include/utilities.hpp"

#include <cstdlib>
#include <cmath>
#include <QTime>

int realRandom()
{
    return abs( rand() * QTime::currentTime().msec() );
}

int *shuffle(int *arrayToShuffle, int numberOfElements)
{
    for (int i = numberOfElements - 1; i > 0; --i)
    {
        int elementToSwap = realRandom() % (i + 1);

        if (elementToSwap != i)
        {
            // Swap
            int temp = arrayToShuffle[i];
            arrayToShuffle[i] = arrayToShuffle[elementToSwap];
            arrayToShuffle[elementToSwap] = temp;
        }
    }

    return arrayToShuffle;
}
