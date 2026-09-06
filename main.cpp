#include "common/FAT32_BPB.h"
#include "common/FAT32_Dir.h"
#include "common/FAT32_FAT.h"   
#include "common/FAT32_Recovery.h"

void directoryNav(FAT32_Directory &dir, FAT32_Recovery &rec);
bool printDet(const FAT32_DirEntry& entry){
    for (int i = 0; i < 11; i++){
        std::cout << entry.name[i];
    }

    std::cout
        << " | Start Cluster: "
        << ((static_cast<uint32_t>(entry.firstClusterHi) << 16) | entry.firstClusterLo)
        << " | Size: "
        << entry.fileSize
        << "\n";
    return true;
}

int main(){
    FAT32_BPB BPB("fat32.img");
    FAT32_FAT FAT(BPB);
    FAT32_Directory directory(BPB, FAT);
    FAT32_Recovery recovery(BPB);
    directoryNav(directory,recovery);
} 