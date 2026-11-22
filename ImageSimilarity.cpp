
#include "ImageSimilarity.h"
int ImageSimilarity::calculateHammingDistance(
    unsigned long long hash1,
    unsigned long long hash2)
{
    unsigned long long difference = hash1 ^ hash2;
    int count = 0;
    while (difference > 0)
    {
        if (difference % 2 == 1)
            count++;
        difference = difference / 2;
    }
    return count;
}
bool ImageSimilarity::areSimilar(
    unsigned long long hash1,
    unsigned long long hash2,
    int threshold)
{
    int distance = calculateHammingDistance(hash1, hash2);
    if (distance <= threshold)
        return true;
    else
        return false;
}