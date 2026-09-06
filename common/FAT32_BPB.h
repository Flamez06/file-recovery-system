#ifndef FAT32_BPB_H
#define FAT32_BPB_H
#include <fstream>
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

class FAT32_BPB
{
private:
    friend class FAT32_FAT;
    friend class FAT32_Directory;
    friend class FAT32_Recovery;
    std::ifstream file;
    uint16_t bytesPerSec;
    uint8_t secPerCluster;
    uint16_t reservedSecCount;
    uint8_t fatCount;
    uint32_t fatSize;
    uint32_t rootClusterNumber;
    uint64_t fatStartOffset;
    uint32_t firstDataSector;
    uint64_t dataStartOffset;
    uint32_t clusterSize;

public:
    explicit FAT32_BPB(const std::string &imagePath);
    uint64_t clusterToOffset(const uint32_t clusterNum) const;
    void printDetails() const;
};

#endif
