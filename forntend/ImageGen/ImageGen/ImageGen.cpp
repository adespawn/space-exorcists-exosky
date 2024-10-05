#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <chrono>

#pragma pack(push, 1) 
struct BMPFileHeader {
    uint16_t fileType{ 0x4D42 };
    uint32_t fileSize{ 0 };
    uint16_t reserved1{ 0 };
    uint16_t reserved2{ 0 };
    uint32_t offsetData{ 54 };
};

struct BMPInfoHeader {
    uint32_t size{ 40 };
    int32_t width{ 0 };
    int32_t height{ 0 };
    uint16_t planes{ 1 };
    uint16_t bitCount{ 24 };
    uint32_t compression{ 0 };
    uint32_t sizeImage{ 0 };
    int32_t xPixelsPerMeter{ 0 };
    int32_t yPixelsPerMeter{ 0 };
    uint32_t colorsUsed{ 0 };
    uint32_t colorsImportant{ 0 };
};

#pragma pack(pop)

struct RGB {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

struct Point {
	int x;
	int y;
};

bool isWhite(int x, int y, std::vector<Point> stars);

void generateBitmap(const char* fileName, int width, std::vector<Point> stars) {
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    infoHeader.width = width;
    infoHeader.height = width;

    // Obliczamy rozmiar danych pikseli (z wyrównaniem do 4 bajtów na każdą linię)
    int rowStride = (width * 3 + 3) & ~3;  // Każda linia musi mieć rozmiar będący wielokrotnością 4 bajtów
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowStride * width;

    std::ofstream file(fileName, std::ios::binary);
    if (file) {
        // Zapisujemy nagłówki
        file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

        RGB blackPixel = { 0, 0, 0 };
		RGB whitePixel = { 255, 255, 255 };
        
        for (int y = 0; y < width; ++y) {
            for (int x = 0; x < width; ++x) {
                if (isWhite(x,y,stars)) {
                    file.write(reinterpret_cast<const char*>(&whitePixel), sizeof(whitePixel));
                }
                else {
                    file.write(reinterpret_cast<const char*>(&blackPixel), sizeof(blackPixel));
                }
            }
            // Dodajemy padding, jeśli jest potrzebny
            uint8_t padding[3] = { 0, 0, 0 };
            file.write(reinterpret_cast<const char*>(padding), rowStride - width * 3);
        }

        file.close();
        std::cout << "Bitmapa została zapisana jako: " << fileName << std::endl;
    }
    else {
        std::cerr << "Nie można utworzyć pliku!" << std::endl;
    }
}

bool isWhite(int x, int y, std::vector<Point> stars) {
    for (auto star : stars)
        if (x == star.x && y == star.y)
            return true;
        else 
            return false;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Point> stars;
    stars.push_back({ 1000,1000 });
    stars.push_back({ 1001,1000 });
    stars.push_back({ 1000,1001 });
    stars.push_back({ 1001,1001 });
    generateBitmap("output.bmp", 2048, stars);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Czas działania: " << duration.count() << " sekund" << std::endl;
	system("output.bmp");

    return 0;
}
