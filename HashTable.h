#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
using namespace std;
class HashTable
{
private:
    struct Entry
    {
        string hash;
        vector<string> filePaths;
        bool occupied;
        Entry()
        {
            occupied = false;
        }
    };
    vector<Entry> table;
    size_t capacity;
    size_t elements;
    int hashFunction(const string& key);
    void rehash();
public:
    HashTable(size_t size = 101);
    void insert(const string& hash, const string& filePath);
    void displayDuplicates();
};
#endif