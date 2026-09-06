#ifndef FAT32_RECOVERY_H
#define FAT32_RECOVERY_H

#include "FAT32_BPB.h"
#include "FAT32_Dir.h"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstdint>

class FAT32_Recovery
{
private:
    FAT32_BPB& BPB;

public:
    FAT32_Recovery(FAT32_BPB& bpb);
    uint32_t getRequiredClusters(uint32_t fileSize);
    std::vector<char> readContiguousFile(uint32_t startCluster,uint32_t fileSize);
};

#endif