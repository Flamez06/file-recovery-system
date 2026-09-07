#include "common/FAT32_BPB.h"
#include "common/FAT32_Dir.h"
#include "common/FAT32_FAT.h"
#include "common/FAT32_Recovery.h"

void directoryNav(FAT32_Directory &dir, FAT32_Recovery &rec);

int main()
{
    FAT32_BPB BPB("fat32.img");
    FAT32_FAT FAT(BPB);
    FAT32_Directory directory(BPB, FAT);
    FAT32_Recovery recovery(BPB);
    directoryNav(directory, recovery);
}