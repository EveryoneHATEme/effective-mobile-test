#include "bmp/Image.h"
#include <sstream>
#include <iostream>

namespace bmp {
    void Image::switchPixel(int x, int y) {
        this->colors[x + y * this->width].red = 255 - this->colors[x + y * this->width].red;
        this->colors[x + y * this->width].green = 255 - this->colors[x + y * this->width].green;
        this->colors[x + y * this->width].blue = 255 - this->colors[x + y * this->width].blue;
    }

    void Image::drawLineLow(int xStart, int yStart, int xEnd, int yEnd) {
        int deltaX = xEnd - xStart;
        int deltaY = yEnd - yStart;
        int yIncrement = 1;

        if (deltaY < 0) {
            yIncrement = -1;
            deltaY = -deltaY;
        }

        int difference = (2 * deltaY) - deltaX;
        int y = yStart;

        for (unsigned int x = xStart; x <= static_cast<uint32_t>(xEnd); x++) {
            if (x >= 0 && x < static_cast<int>(this->width) && y >= 0 && y < static_cast<int>(this->height))
                this->switchPixel(x, y);
            if (difference > 0) {
                y += yIncrement;
                difference += 2 * (deltaY - deltaX);
            } else {
                difference += 2 * deltaY;
            }
        }
    }

    void Image::drawLineHigh(int xStart, int yStart, int xEnd, int yEnd) {
        int deltaX = xEnd - xStart;
        int deltaY = yEnd - yStart;
        int xIncrement = 1;

        if (deltaX < 0) {
            xIncrement = -1;
            deltaX = -deltaX;
        }

        int difference = (2 * deltaX) - deltaY;
        int x = xStart;

        for (unsigned int y = yStart; y <= static_cast<uint32_t>(yEnd); y++) {
            if (x >= 0 && x < static_cast<int>(this->width) && y >= 0 && y < static_cast<int>(this->height))
                this->switchPixel(x, y);
            if (difference > 0) {
                x += xIncrement;
                difference += 2 * (deltaX - deltaY);
            } else {
                difference += 2 * deltaX;
            }
        }
    }

    void Image::read(const std::filesystem::path &filePath) {
        const size_t HEADERS_SIZE = 54;

        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            std::stringstream errorMessageStream;
            errorMessageStream << "Cannot open file " << filePath.c_str();
            throw std::runtime_error(errorMessageStream.str());
        }

        FileHeader fileHeader = FileHeader::read(file);
        InfoHeader infoHeader = InfoHeader::read(file);

        if (fileHeader.signature != 0x4D42) {
            file.close();
            throw std::runtime_error("Given file is not a valid BMP file");
        }
        
        this->width = infoHeader.width;
        this->height = infoHeader.height;
        this->bitCount = infoHeader.bitCount;
        this->horizontalResolution = infoHeader.horizontalResolution;
        this->verticalResolution = infoHeader.verticalResolution;

        // headers take 54 bytes, then before pixel data there is ColorTable that is not used, 
        // so we ignore it
        file.ignore(fileHeader.dataOffset - HEADERS_SIZE);

        this->colors = std::vector<Color>(width * height);

        switch (this->bitCount) {
            case 24:
                for (int y = static_cast<int>(this->height) - 1; y >= 0; y--) {
                    for (int x = 0; x < static_cast<int>(this->width); x++) {
                        this->colors[x + y * this->width] = Color::read24bit(file);
                    }
                }
                break;
            case 32:
                for (int y = static_cast<int>(this->height) - 1; y >= 0; y--) {
                    for (int x = 0; x < static_cast<int>(this->width); x++) {
                        this->colors[x + y * this->width] = Color::read32bit(file);
                    }
                }
                break;
            default:
                file.close();
                throw std::runtime_error("Bit count of this image is unsupported. Only 24-bit and 32-bit images are supported");
                break;
        }
        
        file.close();
    }

    void Image::write(const std::filesystem::path &filePath) const { 
        const size_t HEADERS_SIZE = 54;

        FileHeader fileHeader{};
        fileHeader.signature = 0x4D42;
        fileHeader.fileSize = static_cast<uint32_t>(this->width * this->height * this->bitCount / 8 + HEADERS_SIZE);
        fileHeader.dataOffset = static_cast<uint32_t>(HEADERS_SIZE);

        InfoHeader infoHeader{};
        infoHeader.headerSize = sizeof(InfoHeader);
        infoHeader.width = this->width;
        infoHeader.height = this->height;
        infoHeader.planesCount = static_cast<uint16_t>(1);
        infoHeader.bitCount = static_cast<uint16_t>(this->bitCount);
        infoHeader.compressionType = 0;
        infoHeader.imageSize = this->width * this->height * this->bitCount / 8;
        infoHeader.horizontalResolution = this->horizontalResolution;
        infoHeader.verticalResolution = this->verticalResolution;
        infoHeader.usedColors = 0;
        infoHeader.importantColors = 0;
        
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            std::stringstream errorMessageStream;
            errorMessageStream << "Cannot open file " << filePath.c_str();
            throw std::runtime_error(errorMessageStream.str());
        }

        fileHeader.write(file);
        infoHeader.write(file);

        switch (this->bitCount) {
            case 24:
                for (int y = static_cast<int>(this->height) - 1; y >= 0; y--) {
                    for (int x = 0; x < static_cast<int>(this->width); x++) {
                        this->colors[x + y * this->width].write24bit(file);
                    }
                }
                break;
            case 32:
                for (int y = static_cast<int>(this->height) - 1; y >= 0; y--) {
                    for (int x = 0; x < static_cast<int>(this->width); x++) {
                        this->colors[x + y * this->width].write32bit(file);
                    }
                }
                break;
            default:
                break;
        }

        file.close();
    }

    void Image::drawLine(int xStart, int yStart, int xEnd, int yEnd)
    {
        if (abs(yEnd - yStart) < abs(yEnd - yStart)) {
            if (xStart > xEnd)
                drawLineLow(xEnd, yEnd, xStart, yStart);
            else
                drawLineLow(xStart, yStart, xEnd, yEnd);
        } else {
            if (yStart > yEnd)
                drawLineHigh(xEnd, yEnd, xStart, yStart);
            else
                drawLineHigh(xStart, yStart, xEnd, yEnd);
        }
    }

    void Image::drawCross(int xStart, int yStart, int xEnd, int yEnd) {
        this->drawLine(xStart, yStart, xEnd, yEnd);
        this->drawLine(xStart, yEnd, xEnd, yStart);
    }
    
    void Image::show(std::ostream &destination) const {
        for (unsigned int y = 0; y < this->height; y++) {
            for (unsigned int x = 0; x < this->width; x++) {
                Color currentColor = this->colors[x + y * this->width];
                if (currentColor.red >= 128u && currentColor.green >= 128u && currentColor.blue >= 128u)
                    destination << "█";
                else 
                    destination << " ";
            }
            destination << "\n";
        }
    }
};
