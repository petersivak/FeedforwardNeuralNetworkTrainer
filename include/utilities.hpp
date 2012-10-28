#ifndef UTILITIES_HPP
#define UTILITIES_HPP

int realRandom();

/**
 * @brief The implementation of Fisher-Yates shuffle of an integer array.
 * @param arrayToShuffle array to shuffle
 * @param numberOfElements number of elements of the array
 * @return the shuffled array
 */
int *shuffle(int *arrayToShuffle, int numberOfElements);

#endif // UTILITIES_HPP
