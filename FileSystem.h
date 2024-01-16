#include <iostream>
#include <ctime>
#include <fstream>
#include <filesystem>

#define BLOCK_SIZE 1024
#define MAX_BLOCKS 1024
#define DISK_SIZE (BLOCK_SIZE*MAX_BLOCKS)
#define MAX_INODES 32
#define MAX_BLOCKS_IN_INODE (MAX_BLOCKS/MAX_INODES)

struct Superblock {
    std::size_t diskSize;
    std::size_t blockSize;
    std::size_t diskOccupancy;
    std::size_t numFiles;
    std::size_t freeBlocks;
    time_t lastModificationDate;
};

struct Inode {
    std::size_t dataBlocks[MAX_BLOCKS_IN_INODE];
    time_t creationDate;
    time_t modificationDate;
    std::string permissions;
    std::size_t fileSize;
    bool isDirectory;
    std::size_t referencesCount;
};

class FileSystem {
    std::string diskPath;
    Superblock superblock;
    Inode inodes[MAX_INODES];
    char blocks[MAX_BLOCKS][BLOCK_SIZE];
    bool blocks_usage[MAX_BLOCKS];

public:
    FileSystem(const std::string& diskPath);

    void static createVirtualDisk(const std::string& diskPath);
    void load();
    void save();
    void copyFileToVirtualDisk(std::string systemFilePath, std::string virtualFilePath);
    void createDirectory(std::string directoryPath);
    void removeDirectory(std::string directoryPath);
    void copyFileToSystemDisk(std::string virtualFilePath, std::string systemFilePath);
    void displayDirectoryInfo(std::string directoryPath);
    void createHardLink(std::string sourcePath, std::string destinationPath);
    void removeFileOrLink(std::string path);
    void addBytesToFile(std::string filePath, std::size_t n);
    void truncateFile(std::string filePath, std::size_t n);
    void displayDiskOccupancy();
};
