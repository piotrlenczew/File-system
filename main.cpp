#include "FileSystem.h"
#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc == 3) {
        if(std::strcmp(argv[2], "create") == 0){
            FileSystem::createVirtualDisk(argv[1]);
        }
        else {
            std::cerr << "Incorrect instruction. Can only use createVirtualDisk as 3 argument.";
            return 1;
        }
    }
    else if (argc != 2){
        std::cerr << "Wrong amount of arguments.\n Usage: " << argv[0] << " <virtual_disk_path> <create>\n";
        return 1;
    }

    FileSystem fileSystem(argv[1]);
    fileSystem.load();
    std::string instruction;
    while(instruction != "exit"){
        std::cout << "Pass instruction: ";
        std::cin >> instruction;

        if (instruction == "copyFileToVirtualDisk"){
            std::string systemFilePath;
            std::string virtualFilePath;
            std::cout << "Provide file path: ";
            std::cin >> systemFilePath;
            std::cout << "Provide path where you want to save the file: ";
            std::cin >> virtualFilePath;
            fileSystem.copyFileToVirtualDisk(systemFilePath, virtualFilePath);
        }
        else if (instruction == "createDirectory"){
            std::cout << "Implement method to createDirectory\n";
        }
        else if (instruction == "removeDirectory"){
            std::cout << "Implement method to removeDirectory\n";
        }
        else if (instruction == "copyFileToSystemDisk"){
            std::string systemFilePath;
            std::string virtualFilePath;
            std::cout << "Provide virtual file path: ";
            std::cin >> virtualFilePath;
            std::cout << "Provide path where you want to save the file: ";
            std::cin >> systemFilePath;
            fileSystem.copyFileToSystemDisk(virtualFilePath, systemFilePath);
        }
        else if (instruction == "displayDirectoryInfo"){
            std::cout << "Implement method to displayDirectoryInfo\n";
        }
        else if (instruction == "createHardLink"){
            std::cout << "Implement method to createHardLink\n";
        }
        else if (instruction == "removeFileOrLink"){
            std::cout << "Implement method to removeFileOrLink\n";
        }
        else if (instruction == "addBytesToFile"){
            std::cout << "Implement method to addBytesToFile\n";
        }
        else if (instruction == "truncateFile"){
            std::cout << "Implement method to truncateFile\n";
        }
        else if (instruction == "displayDiskOccupancy"){
            std::cout << "Implement method to displayDiskOccupancy\n";
        }
        else if (instruction != "exit"){
            std::cout << "Unsupported instruction. Try Again.\n";
        }
    }
    fileSystem.save();

    return 0;
}