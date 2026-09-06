#ifndef FAT32_FAT_H
#define FAT32_FAT_H
#include "FAT32_BPB.h"
#include <cstdint>
#include <fstream>
#include <vector>
#include <stdexcept>

class FAT32_FAT {
  private:
    FAT32_BPB &BPB;

  public:
    FAT32_FAT(FAT32_BPB &bpb);
    bool isEndOfChain(uint32_t cluster);
    uint32_t getNextCluster(uint32_t cluster);
    bool isBadCluster(uint32_t cluster);
    std::vector<uint32_t> getClusterChain(uint32_t startCluster);
};
#endif
