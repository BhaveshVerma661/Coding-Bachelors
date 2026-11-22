#pragma once
class ImageSimilarity
{
public:
    static int calculateHammingDistance(
        unsigned long long hash1,
        unsigned long long hash2
    );
    static bool areSimilar(
        unsigned long long hash1,
        unsigned long long hash2,
        int threshold = 10
    );
};