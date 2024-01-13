#include "FileSystem.h"

FileSystem::FileSystem(const std::string& diskPath)
    : diskPath{diskPath}, superblock{}, inodes{}, blocks{}
{

    std::ifstream diskFile(diskPath, std::ios::binary);

    if (diskFile.is_open()) {
        diskFile.read(reinterpret_cast<char*>(&superblock), sizeof(Superblock));
        diskFile.read(reinterpret_cast<char*>(&inodes), sizeof(Inode) * MAX_INODES);
        diskFile.read(reinterpret_cast<char*>(&blocks), sizeof(char) * BLOCK_SIZE * MAX_BLOCKS);
        diskFile.close();
    }
    else {
        std::cerr << "Error: Virtual disk file does not exist at path '" << diskPath << "'." << std::endl;
    }
}

FileSystem::~FileSystem() {
    std::ofstream diskFile(diskPath, std::ios::binary);

    if (diskFile.is_open()) {
        diskFile.write(reinterpret_cast<char*>(&superblock), sizeof(Superblock));
        diskFile.write(reinterpret_cast<char*>(&inodes), sizeof(Inode) * MAX_INODES);
        diskFile.write(reinterpret_cast<char*>(&blocks), sizeof(char) * BLOCK_SIZE * MAX_BLOCKS);
        diskFile.close();
        std::cout << "Virtual disk data saved to file at path '" << diskPath << "'." << std::endl;
    } else {
        std::cerr << "Error: Unable to save virtual disk data to file at path '" << diskPath << "'." << std::endl;
    }
}

void FileSystem::createVirtualDisk(const std::string& diskPath) {
    std::ofstream diskFile(diskPath, std::ios::binary);

    if (diskFile.is_open()) {
        Superblock superblock{};
        superblock.diskSize = DISK_SIZE;
        superblock.blockSize = BLOCK_SIZE;
        superblock.diskOccupancy = 0;
        superblock.numFiles = 0;
        superblock.freeBlocks = MAX_BLOCKS;
        superblock.lastModificationDate = std::time(nullptr);

        Inode inodes[MAX_INODES];
        for (std::size_t i = 0; i < MAX_INODES; ++i) {
            inodes[i] = Inode();
        }

        diskFile.write(reinterpret_cast<char*>(&superblock), sizeof(Superblock));
        diskFile.write(reinterpret_cast<char*>(&inodes), sizeof(Inode) * MAX_INODES);

        char emptyBlock[BLOCK_SIZE] = {};
        for (std::size_t i = 0; i < MAX_BLOCKS; ++i) {
            diskFile.write(emptyBlock, BLOCK_SIZE);
        }

        diskFile.close();
        std::cout << "Virtual disk created successfully at path '" << diskPath << "'." << std::endl;
    } else {
    std::cerr << "Error: Unable to create virtual disk file at path '" << diskPath << "'." << std::endl;
    }
}
