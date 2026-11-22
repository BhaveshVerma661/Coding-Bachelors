#include "FileInfo.h"
FileInfo::FileInfo(string n, string p, string e, long long s)
{
    name = n;
    path = p;
    extension = e;
    size = s;
}
string FileInfo::getName()
{
    return name;
}
string FileInfo::getPath() const
{
    return path;
}
string FileInfo::getExtension() const
{
    return extension;
}
long long FileInfo::getSize()
{
    return size;
}
void FileInfo::setHash(string h)
{
    hash = h;
}
string FileInfo::getHash()
{
    return hash;
} //Bhavesh