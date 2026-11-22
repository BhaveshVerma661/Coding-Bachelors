#ifndef FILEHASHER_H
#define FILEHASHER_H
#include <string>
class FileHasher
{
public:
    static bool calculateHash(
        const std::string& filePath,
        std::string& result
    );
    static bool filesAreEqual(
        const std::string& file1,
        const std::string& file2
    );
};
#endif