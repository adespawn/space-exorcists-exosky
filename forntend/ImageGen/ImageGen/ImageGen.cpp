#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_set>

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

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point& p) const noexcept {
            return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
        }
    };
}

bool isWhite(int x, int y, const std::unordered_set<Point>& starSet) {
    return starSet.find(Point{ x, y }) != starSet.end();
}

// Funkcja do generowania fragmentu bitmapy w pamięci
void generateBitmapSegment(std::vector<std::vector<RGB>>& image, int width, int startY, int endY, const std::unordered_set<Point>& starSet) {
    RGB whitePixel = { 255, 255, 255 };
    RGB blackPixel = { 0, 0, 0 };

    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isWhite(x, y, starSet)) {
                image[y][x] = whitePixel;
            }
            else {
                image[y][x] = blackPixel;
            }
        }
    }
}

void generateBitmap(const char* fileName, int width, const std::unordered_set<Point>& starSet) {
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    infoHeader.width = width;
    infoHeader.height = width;

    // Obliczamy rozmiar danych pikseli (z wyrównaniem do 4 bajtów na każdą linię)
    int rowStride = (width * 3 + 3) & ~3;
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowStride * width;

    // Przygotowanie bitmapy w pamięci (2D tablica RGB)
    std::vector<std::vector<RGB>> image(width, std::vector<RGB>(width));

    int numThreads = std::thread::hardware_concurrency();
    int rowsPerThread = width / numThreads;

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int startY = i * rowsPerThread;
        int endY = (i == numThreads - 1) ? width : startY + rowsPerThread;
        threads.emplace_back(generateBitmapSegment, std::ref(image), width, startY, endY, std::ref(starSet));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::ofstream file(fileName, std::ios::binary);
    if (file) {
        // Zapis nagłówków BMP
        file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

        // Zapis danych pikseli
        for (int y = 0; y < width; ++y) {
            for (int x = 0; x < width; ++x) {
                file.write(reinterpret_cast<const char*>(&image[y][x]), sizeof(RGB));
            }

            // Padding
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

void generateStars(int width, std::unordered_set<Point>& starSet, const int noStars) {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < noStars; ++i) {
        Point star;
        star.x = rand() % width;
        star.y = rand() % width;
        starSet.insert(star);

        // Dodawanie sąsiadów
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                Point neighbor;
                neighbor.x = star.x + dx;
                neighbor.y = star.y + dy;
                if (neighbor.x >= 0 && neighbor.x < width && neighbor.y >= 0 && neighbor.y < width) {
                    starSet.insert(neighbor);
                }
            }
        }
    }
}

int main() {
    const int width = 2048;
    const int noStars = 50000;

    auto start = std::chrono::high_resolution_clock::now();

    std::unordered_set<Point> starSet;
    generateStars(width, starSet, noStars);

    generateBitmap("output.bmp", width, starSet);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Czas działania: " << duration.count() << " sekund" << std::endl;

#ifdef _WIN32
    system("output.bmp");
#endif

    return 0;
}
