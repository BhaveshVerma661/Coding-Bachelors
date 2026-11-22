#include "FileHasher.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <cstring>
using namespace std;
bool FileHasher::calculateHash(
    const string& filePath,
    string& result)
{
    ifstream file(filePath, ios::binary);
    if (!file.is_open())
    {
        cerr << "Unable to open file: "
             << filePath << endl;
        return false;
    }
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;
    char buffer[8192];
    while (file)
    {
        file.read(buffer, sizeof(buffer));
        streamsize bytesRead = file.gcount();
        for (streamsize i = 0; i < bytesRead; i++)
        {
            hash ^= static_cast<unsigned char>(buffer[i]);
            hash *= prime;
        }
    }
    if (file.bad())
    {
        cerr << "Error reading file: "
             << filePath << endl;
        return false;
    }
    stringstream ss;
    ss << hex << setfill('0')
       << setw(16) << hash;
    result = ss.str();
    return true;
}
bool FileHasher::filesAreEqual(
    const string& file1,
    const string& file2)
{
    ifstream f1(file1, ios::binary);
    ifstream f2(file2, ios::binary);
    if (!f1.is_open() || !f2.is_open())
    {
        return false;
    }
    char buffer1[8192];
    char buffer2[8192];
    while (true)
    {
        f1.read(buffer1, sizeof(buffer1));
        f2.read(buffer2, sizeof(buffer2));
        streamsize count1 = f1.gcount();
        streamsize count2 = f2.gcount();
        if (count1 != count2)
        {
            return false;
        }
        if (memcmp(buffer1, buffer2, count1) != 0)
        {
            return false;
        }
        if (f1.eof() && f2.eof())
        {
            return true;
        }
        if (f1.bad() || f2.bad())
        {
            return false;
        }
    }
}