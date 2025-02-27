#ifndef BMPIMAGE_H
#define BMPIMAGE_H

#include "Structs.h"

#include <filesystem>
#include <fstream>
#include <vector>


namespace bmp {
    class Image {
    private:
        unsigned int width, height;
        unsigned int bitCount;
        unsigned int horizontalResolution, verticalResolution;

        std::vector<Color> colors;

        void switchPixel(int x, int y);
        void drawLineLow(int xStart, int yStart, int xEnd, int yEnd);
        void drawLineHigh(int xStart, int yStart, int xEnd, int yEnd);

    public:
        Image() = default;
        Image(const std::filesystem::path& filePath) {
            this->read(filePath);
        }

        void read(const std::filesystem::path& filePath);
        void write(const std::filesystem::path& filePath) const;

        void drawLine(int xStart, int yStart, int xEnd, int yEnd);
        void drawCross(int xStart, int yStart, int xEnd, int yEnd);

        void show(std::ostream& destination) const;

        unsigned int getWidth() const {
            return this->width;
        }

        unsigned int getHeight() const {
            return this->height;
        }
    };
};


#endif
