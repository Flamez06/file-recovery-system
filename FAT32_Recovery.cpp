#include "common/FAT32_Recovery.h"
FAT32_Recovery::FAT32_Recovery(FAT32_BPB& bpb) : BPB(bpb) {}

uint32_t FAT32_Recovery::getRequiredClusters(uint32_t fileSize){
    return (fileSize + BPB.clusterSize - 1) / BPB.clusterSize;
}

//Read n contiguous clusters starting from startCluster and return the data as a vector of chars
std::vector<char> FAT32_Recovery::readContiguousFile(uint32_t startCluster,uint32_t fileSize){
    uint32_t requiredClusters = getRequiredClusters(fileSize);
    std::vector<char> fileData(fileSize);
    uint32_t bytesRead=0;
    for(uint32_t i=0;i<requiredClusters;i++){
        if (!BPB.file) {
            throw std::runtime_error("File stream is not open");
        }

        uint64_t offset = BPB.clusterToOffset(startCluster+i);
        BPB.file.clear();
        BPB.file.seekg(offset);
        uint32_t bytesToRead = std::min(BPB.clusterSize, fileSize - bytesRead);
        BPB.file.read(fileData.data() + bytesRead, bytesToRead);
        bytesRead += bytesToRead;
    }
    return fileData;
}