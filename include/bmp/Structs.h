#ifndef BMPSTRUCTS_H
#define BMPSTRUCTS_H

#include <cstdint>
#include <fstream>


namespace bmp {
    template<typename T>
    void readBinary(std::istream& source, T& value) {
        source.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    template<typename... StructFields>
    void readStructFields(std::istream& source, StructFields&... structFields) {
        (readBinary(source, structFields), ...);
    } 

    template<typename T>
    void writeBinary(std::ostream& destination, T& value) {
        destination.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    template<typename... StructFields>
    void writeStructFields(std::ostream& destination, StructFields&... structFields) {
        (writeBinary(destination, structFields), ...);
    } 

    struct FileHeader {
        uint16_t signature;
        uint32_t fileSize;
        uint32_t unused;
        uint32_t dataOffset;

        static FileHeader read(std::istream& source) {
            FileHeader instance{};
            readStructFields(source, instance.signature, instance.fileSize, instance.unused, instance.dataOffset);
            return instance;
        }

        void write(std::ofstream& destination) {
            writeStructFields(destination, this->signature, this->fileSize, this->unused, this->dataOffset);
        }
    };
    
    struct InfoHeader {
        uint32_t headerSize;
        uint32_t width;
        uint32_t height;
        uint16_t planesCount;
        uint16_t bitCount;
        uint32_t compressionType;
        uint32_t imageSize;
        uint32_t horizontalResolution;
        uint32_t verticalResolution;
        uint32_t usedColors;
        uint32_t importantColors;

        static InfoHeader read(std::istream& source) {
            InfoHeader instance{};
            readStructFields(
                source, 
                instance.headerSize, 
                instance.width, 
                instance.height,
                instance.planesCount, 
                instance.bitCount, 
                instance.compressionType,
                instance.imageSize, 
                instance.horizontalResolution,
                instance.verticalResolution, 
                instance.usedColors,
                instance.importantColors
            );
            return instance;
        }
        
        void write(std::ofstream& destination) {
            writeStructFields(
                destination, 
                this->headerSize, 
                this->width, 
                this->height,
                this->planesCount, 
                this->bitCount, 
                this->compressionType,
                this->imageSize, 
                this->horizontalResolution,
                this->verticalResolution, 
                this->usedColors,
                this->importantColors
            );
        }
    };

    struct Color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha = 255u; 

        static Color read24bit(std::istream& source) {
            Color instance{};
            readStructFields(source, instance.blue, instance.green, instance.red);
            return instance;
        }

        static Color read32bit(std::istream& source) {
            Color instance{};
            readStructFields(source, instance.blue, instance.green, instance.red, instance.alpha);
            return instance;
        }

        void write24bit(std::ofstream& destination) const {
            writeStructFields(destination, this->blue, this->green, this->red);
        }

        void write32bit(std::ofstream& destination) const {
            writeStructFields(destination, this->blue, this->green, this->red, this->alpha);
        }
    };
};

#endif
