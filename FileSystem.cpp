#include "FileSystem.h"

FileSystem::FileSystem(const std::string& diskPath) : diskPath(diskPath)
{
    superblock.diskSize = DISK_SIZE;
    superblock.blockSize = BLOCK_SIZE;
    superblock.diskOccupancy = 0;
    superblock.numFiles = 0;
    superblock.freeBlocks = MAX_BLOCKS;
    superblock.lastModificationDate = std::time(nullptr);
}

void FileSystem::createVirtualDisk(const std::string& diskPath) {
    std::ofstream diskFile(diskPath, std::ios::binary);

    if (diskFile.is_open()) {
        Superblock new_superblock{};
        new_superblock.diskSize = DISK_SIZE;
        new_superblock.blockSize = BLOCK_SIZE;
        new_superblock.diskOccupancy = 0;
        new_superblock.numFiles = 0;
        new_superblock.freeBlocks = MAX_BLOCKS;
        new_superblock.lastModificationDate = std::time(nullptr);
        Inode new_inodes[MAX_INODES];
        char new_block[BLOCK_SIZE];
        bool new_blocks_usage[MAX_BLOCKS];
        new_blocks_usage[0] = true;
        for (int i = 1; i < MAX_BLOCKS; ++i) {
            new_blocks_usage[i] = false;
        }
        Inode root{};
        root.dataBlocks[0] = 0;
        root.isDirectory = true;
        const char rootEntry[BLOCK_SIZE] = {"`0/..`0/.`"};

        new_inodes[0] = root;
        diskFile.write(reinterpret_cast<char*>(&new_superblock), sizeof(Superblock));
        diskFile.write(reinterpret_cast<char*>(new_inodes), sizeof(Inode) * MAX_INODES);
        diskFile.write(rootEntry, sizeof(char) * BLOCK_SIZE);
        for (int i = 0; i < MAX_BLOCKS - 1; ++i) {
            diskFile.write(new_block, sizeof(char) * BLOCK_SIZE);
        }
        diskFile.write(reinterpret_cast<char*>(&new_blocks_usage), sizeof(bool) * MAX_BLOCKS);
        diskFile.close();
        std::cout << "Virtual disk created successfully at path '" << diskPath << "'." << std::endl;
    } else {
        std::cerr << "Error: Unable to create virtual disk file at path '" << diskPath << "'." << std::endl;
    }
}

void FileSystem::load() {
    std::ifstream diskFile(diskPath, std::ios::binary);

    if (diskFile.is_open()) {
        char temp_block[BLOCK_SIZE];
        diskFile.read(reinterpret_cast<char*>(&superblock), sizeof(Superblock));
        diskFile.read(reinterpret_cast<char*>(&inodes), sizeof(Inode) * MAX_INODES);
        for (int i = 0; i < MAX_BLOCKS; ++i) {
            diskFile.read(temp_block, sizeof(char) * BLOCK_SIZE);

            for (int j = 0; j < BLOCK_SIZE; ++j) {
                blocks[i][j] = temp_block[j];
            }
        }
        diskFile.read(reinterpret_cast<char*>(&blocks_usage), sizeof(bool) * MAX_BLOCKS);
        diskFile.close();

        auto fileSize = static_cast<std::streampos>(sizeof(Superblock) + sizeof(Inode) * MAX_INODES +
                                                              sizeof(char) * BLOCK_SIZE * MAX_BLOCKS + sizeof(bool) * MAX_BLOCKS);

        if (fileSize != std::filesystem::file_size(diskPath)) {
            std::cerr << "Error: Incomplete or corrupted virtual disk file at path '" << diskPath << "'." << std::endl;
        }

        std::cout << "Virtual disk loaded successfully from path '" << diskPath << "'." << std::endl;
    }
    else {
        std::cerr << "Error: Virtual disk file does not exist at path '" << diskPath << "'." << std::endl;
    }
}

void FileSystem::save() {
    std::ofstream diskFile(diskPath, std::ios::binary);

    if (diskFile.is_open()) {
        char temp_block[BLOCK_SIZE];
        diskFile.write(reinterpret_cast<char*>(&superblock), sizeof(Superblock));
        diskFile.write(reinterpret_cast<char*>(&inodes), sizeof(Inode) * MAX_INODES);
        for (int i = 0; i < MAX_BLOCKS; ++i) {
            for (int j = 0; j < BLOCK_SIZE; ++j) {
                temp_block[j] = blocks[i][j];
            }

            diskFile.write(temp_block, sizeof(char) * BLOCK_SIZE);
        }
        diskFile.write(reinterpret_cast<char*>(&blocks_usage), sizeof(bool) * MAX_BLOCKS);
        diskFile.close();
        std::cout << "Virtual disk data saved to file at path '" << diskPath << "'." << std::endl;
    } else {
        std::cerr << "Error: Unable to save virtual disk data to file at path '" << diskPath << "'." << std::endl;
    }
}

void FileSystem::copyFileToVirtualDisk(std::string systemFilePath, std::string virtualFilePath) {
    std::ifstream sourceFile(systemFilePath, std::ios::binary);
    if (!sourceFile.is_open()) {
        std::cerr << "Error: Unable to open source file '" << systemFilePath << "'." << std::endl;
        return;
    }
}
