#include "common/FAT32_BPB.h"
#include "common/FAT32_Dir.h"
#include "common/RecoveryHeader.h"
#include "common/FAT32_Recovery.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <optional>
#include <algorithm>

std::unordered_map<uint32_t, FAT32_DirEntry> deletedFilesMap; // Map to store deleted files with their starting cluster as the key
std::string formatFilename(const FAT32_DirEntry &entry)       // LFN NOT HANDLED YET
{
    uint8_t cleanName[11];
    std::copy(std::begin(entry.name), std::end(entry.name), cleanName);
    if (cleanName[0] == 0xE5)
    {
        cleanName[0] = '_'; // placeholder char to replace the deleted char
    }
    std::string base(reinterpret_cast<const char *>(cleanName), 8);
    base.erase(base.find_last_not_of(' ') + 1);
    std::string ext(reinterpret_cast<const char *>(cleanName + 8), 3);
    ext.erase(ext.find_last_not_of(' ') + 1);

    if (!ext.empty())
    {
        return base + "." + ext;
    }
    return base;
}

void recoverFile(int serialNumber, FAT32_Recovery &recovery, FAT32_Directory &dir)
{
    auto it = deletedFilesMap.find(serialNumber);
    if (it == deletedFilesMap.end())
    {
        if (deletedFilesMap.empty()) // map must be initialized first
        {
            std::cerr << "You must ls in this directory atleast once before attempting recovery \n";
            return;
        }
        else
        {
            std::cerr << "ERROR: Invalid serial number\n";
            return;
        }
    }

    const FAT32_DirEntry &entry = it->second;
    uint32_t startCluster = dir.getFirstCluster(entry);
    uint32_t fileSize = entry.fileSize;

    try
    {
        std::vector<char> fileData = recovery.readContiguousFile(startCluster, fileSize);
        std::string filename = "RECOVERED" + formatFilename(entry);

        std::string outputPath = "recovered/" + filename;
        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile)
        {
            std::cerr << "ERROR: Could not create output file\n";
            return;
        }
        outFile.write(fileData.data(), fileData.size());
        outFile.close();
        std::cout << "File recovered successfully: " << filename << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
    }
}

void listDirectory(FAT32_Directory &dir, uint32_t clusterNumber)
{
    deletedFilesMap.clear(); // Clear the map before listing the directory
    std::cout << std::left
              << std::setw(10) << "[SL_NO.]"
              << std::setw(25) << "[Name]"
              << std::setw(12) << "[Type]"
              << std::setw(15) << "[Size]"
              << "[Status]" << std::endl;           // status shows active/deleted
    std::cout << std::string(65, '-') << std::endl; // just terminal formatting
    uint32_t serialNumber = 1;
    dir.walkDirectory(clusterNumber, [&dir, &serialNumber](const FAT32_DirEntry &entry)
                      {
        if (entry.name[0] == '.') return true;
        bool isDel = dir.isDeleted(entry);
        bool isDir = dir.isDirectory(entry);

        if(isDel && isDir) return true;
        if(isDel) {
            deletedFilesMap[serialNumber] = entry; // Store deleted file in the map
        }
        std::string serialStr = std::to_string(serialNumber);
        std::string filename = formatFilename(entry);
        std::string typeStr = isDir? "<DIR>":"<FILE>";
        std::string statusStr = isDel? "Deleted":"Active";
        std::string sizeStr = std::to_string(entry.fileSize) + " B";

        std::cout << std::left 
                  << std::setw(10) << serialNumber++
                  << std::setw(25) << filename 
                  << std::setw(12) << typeStr 
                  << std::setw(15) << sizeStr
                  << statusStr << std::endl; 
        return true; });
}

std::optional<uint32_t> changeDirectory(FAT32_Directory &dir, uint32_t currentCluster, std::string &arg)
{
    if (arg == ".")
        return currentCluster;

    uint32_t targetCluster = 0;
    bool dirFound = false;
    dir.walkDirectory(currentCluster, [&dir, &arg, &targetCluster, &dirFound](const FAT32_DirEntry &entry)
                      {
        if(dir.isDirectory(entry)){
            if(formatFilename(entry) == arg){
                targetCluster = dir.getFirstCluster(entry); 
                dirFound = true;
                return false;   
            }
            return true;
        }
    return true; });

    if (dirFound)
    {
        deletedFilesMap.clear(); // prevent stale map from carrying through to next directory
        return targetCluster;
    }

    return std::nullopt;
}

void directoryNav(FAT32_Directory &dir, FAT32_Recovery &rec)
{
    uint32_t currentCluster = dir.getRootCluster();

    while (true)
    {
        std::cout << "\n[Directory Navigator] : [Current Cluster]:" << currentCluster << std::endl;
        std::string command, arg;
        std::cin >> command;

        if (command == "ls")
        {
            listDirectory(dir, currentCluster);
        }
        else if (command == "recover")
        {
            std ::cin >> arg;
            try
            {
                int serial = std::stoi(arg);
                recoverFile(serial, rec, dir);
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "ERROR: Invalid serial. Please enter a valid number\n";
            }
            catch (const std::out_of_range &e)
            {
                std::cerr << "ERROR: Serial number too large\n";
            }
        }
        else if (command == "cd")
        {
            std::cin >> arg;
            auto nextClusterOpt = changeDirectory(dir, currentCluster, arg);

            if (nextClusterOpt.has_value())
            {
                uint32_t nextCluster = nextClusterOpt.value();

                if (nextCluster == 0)
                {
                    currentCluster = dir.getRootCluster();
                }
                else
                {
                    currentCluster = nextCluster;
                }
            }

            else
            {
                std::cerr << "ERROR: Directory not found\n";
            }
        }
        else if (command == "exit")
        {
            std::cout << "Exiting...\n";
            break;
        }
        else
        {
            std::cerr << "ERROR: Unknown command\n";
        }
    }
}
