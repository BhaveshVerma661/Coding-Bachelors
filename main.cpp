#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <utility>
#include <fstream>
#include <sstream>
#include "FileInfo.h"
#include "FileScanner.h"
#include "FileHasher.h"
#include "HashTable.h"
#include "Chunker.h"
#include "PerceptualHasher.h"
#include "ImageSimilarity.h"
using namespace std;
bool isImageFile(const string& extension)
{
    string ext = extension;
    transform(
        ext.begin(),
        ext.end(),
        ext.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(tolower(c));
        }
    );
    return ext == ".jpg"  ||
           ext == ".jpeg" ||
           ext == ".png"  ||
           ext == ".bmp"  ||
           ext == ".tif"  ||
           ext == ".tiff" ||
           ext == ".webp";
}
int main()
{
    string folderPath;
    cout << "Enter folder path: ";
    getline(cin, folderPath);
    ofstream report("Deduplication_Report.txt");
    if (!report.is_open())
    {
        cerr << "Error: Could not create report file.\n";
        return 1;
    }
    report << "       FILE DEDUPLICATION REPORT\n";
    report << "Folder Scanned: " << folderPath << "\n\n";
    FileScanner scanner;
    vector<FileInfo> files =
        scanner.scanFolder(folderPath);
    if (files.empty())
    {
        cout << "\nNo files found in the folder.\n";
        report << "No files found in the specified folder.\n";
        report.close();
        cout << "\nReport created: Deduplication_Report.txt\n";
        return 0;
    }
    cout << "\nTotal files found: "
         << files.size() << endl;
    report << "Total Files Found: "
           << files.size() << "\n\n";
    auto captureSection = [&](const string& heading, auto action)
    {
        cout << "\n" << heading << "\n";
        report << "\n" << heading << "\n";
        ostringstream captured;
        streambuf* oldBuffer = cout.rdbuf(captured.rdbuf());
        action();
        cout.rdbuf(oldBuffer);
        cout << captured.str();
        report << captured.str();
    };
    HashTable duplicateTable;
    captureSection(
        "EXACT DUPLICATE DETECTION",
        [&]()
        {
            cout << "Calculating file hashes...\n";
            for (FileInfo& file : files)
            {
                string filePath = file.getPath();
                string hash;
                if (FileHasher::calculateHash(filePath, hash))
                {
                    file.setHash(hash);
                    duplicateTable.insert(
                        file.getHash(),
                        filePath
                    );
                }
                else
                {
                    cout << "Skipping file: "
                         << filePath << endl;
                }
            }
            duplicateTable.displayDuplicates();
        }
    );
    cout << "\n IMAGE SIMILARITY \n";
    report << "\n IMAGE SIMILARITY\n";
    vector<pair<string, unsigned long long>> imageHashes;
    for (const FileInfo& file : files)
    {
        if (!isImageFile(file.getExtension()))
        {
            continue;
        }
        unsigned long long hash = 0;
        if (PerceptualHasher::calculateHash(
                file.getPath(), hash))
        {
            imageHashes.push_back(
                {file.getPath(), hash}
            );
        }
    }
    cout << "\nTotal valid images processed: "
         << imageHashes.size() << endl;
    report << "\nTotal valid images processed: "
           << imageHashes.size() << "\n";
    bool similarImagesFound = false;
    int similarImagePairs = 0;
    ostringstream imageResults;
    for (size_t i = 0; i < imageHashes.size(); i++)
    {
        for (size_t j = i + 1; j < imageHashes.size(); j++)
        {
            int distance =
                ImageSimilarity::calculateHammingDistance(
                    imageHashes[i].second,
                    imageHashes[j].second
                );
            if (ImageSimilarity::areSimilar(
                    imageHashes[i].second,
                    imageHashes[j].second))
            {
                imageResults << "\nSimilar Images Found:\n";
                imageResults << "Image 1: "
                             << imageHashes[i].first << "\n";
                imageResults << "Image 2: "
                             << imageHashes[j].first << "\n";
                imageResults << "Hamming Distance: "
                             << distance << "\n";
                similarImagesFound = true;
                similarImagePairs++;
            }
        }
    }
    if (!similarImagesFound)
    {
        imageResults << "\nNo visually similar images found.\n";
    }
    cout << imageResults.str();
    report << imageResults.str();
    vector<string> filePaths;
    for (const FileInfo& file : files)
    {
        filePaths.push_back(file.getPath());
    }
    Chunker chunker;
    captureSection(
        " FILE CHUNK SIMILARITY DETECTION",
        [&]()
        {
            chunker.findChunks(filePaths);
        }
    );
    cout << "\n FINAL SUMMARY \n";
    cout << "Total files scanned: "
         << files.size() << endl;
    cout << "Valid images processed: "
         << imageHashes.size() << endl;
    cout << "Similar image pairs found: "
         << similarImagePairs << endl;
    report << "FINAL SUMMARY\n";
    report << "Total files scanned: "
           << files.size() << "\n";
    report << "Valid images processed: "
           << imageHashes.size() << "\n";
    report << "Similar image pairs found: "
           << similarImagePairs << "\n";
    report << "\nReport generation completed successfully.\n";
    report.close();
    cout << "\nReport generated successfully!";
    cout << "\nFile: Deduplication_Report.txt\n";
    cout << "\nProgram completed successfully.\n";
    return 0;
}