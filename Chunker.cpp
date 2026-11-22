#include "Chunker.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <openssl/sha.h>
using namespace std;
void Chunker::findChunks(const vector<string>& filePaths)
{
    map<string, vector<pair<string, int>>> fileChunks;
    map<string, long long> fileSizes;
    for (string path : filePaths)
    {
        ifstream file(path, ios::binary);
        if (!file)
            continue;
        char data[4096];
        fileSizes[path] = 0;
        while (file.read(data, 4096) || file.gcount() > 0)
        {
            int size = file.gcount();
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256((unsigned char*)data, size, hash);
            string hashString;
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
            {
                hashString += to_string(hash[i]) + " ";
            }
            fileChunks[path].push_back({hashString, size});
            fileSizes[path] += size;
        }
    }
    cout << "\nFILE SIMILARITY\n";
    bool found = false;
    for (auto a = fileChunks.begin(); a != fileChunks.end(); a++)
    {
        auto b = a;
        b++;
        for (; b != fileChunks.end(); b++)
        {
            long long matchingBytes = 0;
            vector<bool> used(b->second.size(), false);
            for (auto chunkA : a->second)
            {
                for (int i = 0; i < b->second.size(); i++)
                {
                    if (!used[i] && chunkA.first == b->second[i].first)
                    {
                        matchingBytes += chunkA.second;
                        used[i] = true;
                        break;
                    }
                }
            }
            long long largerSize = max(
                fileSizes[a->first],
                fileSizes[b->first]
            );
            if (matchingBytes > 0 && largerSize > 0)
            {
                double similarity =
                    (double)matchingBytes / largerSize * 100;
                cout << a->first << " <-> " << b->first
                     << " : " << fixed << setprecision(2)
                     << similarity << "% similar\n";
                found = true;
            }
        }
    }
    if (!found)
    {
        cout << "No similar files found.\n";
    }
}