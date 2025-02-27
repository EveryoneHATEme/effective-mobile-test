#include <iostream>
#include <fstream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

#include "bmp/Image.h"


int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    std::cout << "Enter input BMP file name: ";

    std::string pathString;
    std::cin >> pathString;
    std::filesystem::path filePath(pathString);
    
    bmp::Image img(filePath);

    std::cout << "Obtained image:" << std::endl;
    img.show(std::cout);
    std::cout << std::endl << std::endl;

    img.drawCross(0, 0, img.getWidth(), img.getHeight());

    std::cout << "Crossed image:" << std::endl;
    img.show(std::cout);

    std::cout << "Enter output BMP file name: ";
    std::cin >> pathString;
    filePath = std::filesystem::path(pathString);

    img.write(pathString);
    
    return 0;
} 