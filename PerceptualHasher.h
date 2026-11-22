#pragma once
#include <string>
#include <opencv2/opencv.hpp>
class PerceptualHasher
{
public:
    static bool calculateHash(
        const std::string& imagePath,
        unsigned long long& hash
    );
};