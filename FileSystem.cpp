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
        root.referencesCount = 1;
        const char rootEntry[BLOCK_SIZE] = {"{0/..}"};

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

void FileSystem::copyFileToVirtualDisk(const std::string& systemFilePath, const std::string& virtualFilePath) {
    std::ifstream sourceFile(systemFilePath, std::ios::binary);
    if (!sourceFile.is_open()) {
        std::cerr << "Error: Unable to open source file '" << systemFilePath << "'." << std::endl;
        return;
    }

    std::vector<int> freeBlockIndices;
    sourceFile.seekg(0, std::ios::end);
    int requiredBlocks = sourceFile.tellg() / BLOCK_SIZE + 1;
    sourceFile.seekg(0, std::ios::beg);
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        if (!blocks_usage[i]) {
            freeBlockIndices.push_back(i);
            blocks_usage[i] = true;

            if (freeBlockIndices.size() == requiredBlocks) {
                break;
            }
        }
    }

    if (freeBlockIndices.size() < requiredBlocks) {
        std::cerr << "Error: Not enough free blocks available in the virtual disk." << std::endl;
        return;
    }

    Inode virtualFileInode;
    virtualFileInode.creationDate = std::time(nullptr);
    virtualFileInode.modificationDate = virtualFileInode.creationDate;
    virtualFileInode.fileSize = sourceFile.tellg();
    virtualFileInode.isDirectory = false;
    virtualFileInode.referencesCount = 1;

    int freeInodeIndex = -1;
    for (int i = 0; i < MAX_INODES; ++i) {
        if (inodes[i].referencesCount == 0) {
            freeInodeIndex = i;
            inodes[i] = virtualFileInode;
            break;
        }
    }

    //update of root directory
    int start_index = -1;
    char current_character = 'a';
    while (current_character != '\000'){
        start_index ++;
        current_character = blocks[0][start_index];
    }
    const std::string rootEntry = "{" + std::to_string(freeInodeIndex) + "/" + virtualFilePath + "}";
    const char* rootEntryChars = rootEntry.c_str();
    for (int i = start_index; i < rootEntry.size()+start_index; ++i) {
        blocks[0][i] = rootEntryChars[i - start_index];
    }


    if (freeInodeIndex == -1) {
        std::cerr << "Error: No free inodes available in the virtual disk." << std::endl;
        return;
    }

    std::size_t remainingBytes = sourceFile.tellg();
    std::size_t j = 0;
    for (int blockIndex : freeBlockIndices) {
        std::size_t bytesRead = sourceFile.readsome(blocks[blockIndex], BLOCK_SIZE);
        remainingBytes -= bytesRead;
        superblock.freeBlocks--;

        virtualFileInode.dataBlocks[j] = blockIndex;

        if (remainingBytes == 0) {
            break;
        }
    }
    superblock.numFiles++;

    std::cout << "File copied successfully from '" << systemFilePath << "' to virtual disk path '" << virtualFilePath << "'." << std::endl;
}
