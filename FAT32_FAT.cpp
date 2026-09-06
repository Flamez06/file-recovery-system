#include "common/FAT32_FAT.h"
#include "common/RecoveryHeader.h"

FAT32_FAT::FAT32_FAT(FAT32_BPB &bpb) : BPB(bpb) {}

uint32_t FAT32_FAT::getNextCluster(uint32_t cluster)
{
    uint64_t fatEntryOffset = BPB.fatStartOffset + (uint64_t)cluster * 4;
    BPB.file.clear();
    BPB.file.seekg(fatEntryOffset);
    uint32_t value;
    BPB.file.read((char *)&value, sizeof(value));
    if (!BPB.file)
    {
        throw std::runtime_error("Failed to read FAT entry");
    }
    value &= FAT32_CLUSTER_MASK; // Mask and remove first 4 bits as only the next 28 are useful
    return value;
}

bool FAT32_FAT::isEndOfChain(uint32_t cluster)
{
    return cluster >= FAT32_END_OF_CHAIN;
}

bool FAT32_FAT::isBadCluster(uint32_t cluster)
{
    return cluster == FAT32_BAD_CLUSTER;
}

std::vector<uint32_t> FAT32_FAT::getClusterChain(uint32_t startCluster)
{
    std::vector<uint32_t> chain;
    uint32_t currentCluster = startCluster;
    while (true)
    {
        chain.push_back(currentCluster);
        uint32_t nextCluster = getNextCluster(currentCluster);
        if (isEndOfChain(nextCluster))
        {
            break;
        }
        if (isBadCluster(nextCluster))
        {
            throw std::runtime_error("Bad cluster encountered");
        }
        currentCluster = nextCluster;
    }
    return chain;
}
