
#include "PerceptualHasher.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace cv;
bool PerceptualHasher::calculateHash(
    const string& imagePath,
    unsigned long long& hash)
{
    Mat image = imread(imagePath, IMREAD_COLOR);
    if (image.empty())
    {
        cerr << "Error: Cannot read image: " << imagePath << endl;
        return false;
    }
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    Mat resized;
    resize(gray, resized, Size(32, 32));
    Mat floatImage;
    resized.convertTo(floatImage, CV_32F);
    Mat dctImage;
    dct(floatImage, dctImage);
    Mat lowFrequency = dctImage(Rect(0, 0, 8, 8));
    vector<float> values;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i == 0 && j == 0)
                continue;
            values.push_back(lowFrequency.at<float>(i, j));
        }
    }
    sort(values.begin(), values.end());
    float median = values[values.size() / 2];
    hash = 0;
    int bit = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (lowFrequency.at<float>(i, j) > median)
            {
                hash |= (1ULL << bit);
            }
            bit++;
        }
    }
    return true;
}