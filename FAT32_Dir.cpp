#include "common/FAT32_Dir.h"

FAT32_Directory::FAT32_Directory(FAT32_BPB &bpb, FAT32_FAT &fat) : BPB(bpb), FAT(fat) {}

// Reconstruct first cluster from hi lo
uint32_t FAT32_Directory::getFirstCluster(const FAT32_DirEntry &entry) {
    return (static_cast<uint32_t>(entry.firstClusterHi) << 16) | entry.firstClusterLo;
}

uint32_t FAT32_Directory::getRootCluster() const { return BPB.rootClusterNumber; };

bool FAT32_Directory::isEndOfDirectory(const FAT32_DirEntry &entry) { return entry.name[0] == 0x00; }

bool FAT32_Directory::isDeleted(const FAT32_DirEntry &entry) { return entry.name[0] == 0xE5; }

bool FAT32_Directory::isLFN(const FAT32_DirEntry &entry) { return entry.attr == 0x0F; }

bool FAT32_Directory::isDirectory(const FAT32_DirEntry &entry) { return (entry.attr & 0x10) != 0; }

void FAT32_Directory::walkDirectory(uint32_t startCluster, std::function<bool(const FAT32_DirEntry &)> callback) {
    std::vector<uint32_t> chain = FAT.getClusterChain(startCluster);
    for (auto cluster : chain) {
        uint64_t offset = BPB.clusterToOffset(cluster);
        BPB.file.clear();
        BPB.file.seekg(offset);

        uint32_t entriesPerCluster = BPB.clusterSize / sizeof(FAT32_DirEntry);
        for (uint32_t i = 0; i < entriesPerCluster; i++) {
            FAT32_DirEntry entry{};
            BPB.file.read((char *)&entry, sizeof(FAT32_DirEntry));
            if (!BPB.file) {
                throw std::runtime_error("Failed to seek directory cluster");
            }
            if (isEndOfDirectory(entry)) {
                return;
            }
            if (isLFN(entry)) {
                continue;
            }
            callback(entry);
        }
    }
}