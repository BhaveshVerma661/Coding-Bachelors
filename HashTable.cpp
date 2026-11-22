#include "HashTable.h"
#include "FileHasher.h"
HashTable::HashTable(size_t size)
{
    capacity = size;
    if (capacity == 0)
        capacity = 101;
    elements = 0;
    table.resize(capacity);
}
int HashTable::hashFunction(const string& key)
{
    unsigned long long hash = 0;
    for (char ch : key)
    {
        hash = hash * 31 + ch;
    }
    return hash % capacity;
}
void HashTable::insert(const string& hash, const string& filePath)
{
    int index = hashFunction(hash);
    while (table[index].occupied)
    {
        if (table[index].hash == hash)
        {
            for (string path : table[index].filePaths)
            {
                if (path == filePath)
                    return;
                if (FileHasher::filesAreEqual(path, filePath))
                {
                    table[index].filePaths.push_back(filePath);
                    return;
                }
            }
        }
        index = (index + 1) % capacity;
    }
    if ((elements + 1) * 10 >= capacity * 7)
    {
        rehash();
        insert(hash, filePath);
        return;
    }
    table[index].hash = hash;
    table[index].filePaths.push_back(filePath);
    table[index].occupied = true;
    elements++;
}
void HashTable::rehash()
{
    vector<Entry> oldTable = table;
    capacity = capacity * 2 + 1;
    table.clear();
    table.resize(capacity);
    elements = 0;
    for (Entry entry : oldTable)
    {
        if (entry.occupied)
        {
            for (string path : entry.filePaths)
            {
                insert(entry.hash, path);
            }
        }
    }
}
void HashTable::displayDuplicates()
{
    bool found = false;
    cout << "\nDUPLICATE FILES\n";
    for (Entry entry : table)
    {
        if (entry.occupied && entry.filePaths.size() > 1)
        {
            found = true;
            cout << "\nDuplicate Group:\n";
            for (string path : entry.filePaths)
            {
                cout << path << endl;
            }
        }
    }
    if (!found)
    {
        cout << "\nNo duplicate files found.\n";
    }
}