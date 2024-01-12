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
}
