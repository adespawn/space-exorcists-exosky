#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <string>
#include <sstream>

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

struct dane {
    long long sun_id;
    double x;
    double y;
    double z;
    double intensity;
};

namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point& p) const noexcept {
            return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
        }
    };
}

double map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool isWhite(int x, int y, const std::unordered_set<Point>& starSet) {
    return starSet.find(Point{ x, y }) != starSet.end();
}
dane minVals(const std::vector<dane>& dataset) {
    dane min = dataset[0];
    for (const auto& d : dataset) {
        if (d.x < min.x) min.x = d.x;
        if (d.y < min.y) min.y = d.y;
        if (d.z < min.z) min.z = d.z;
        if (d.intensity < min.intensity) min.intensity = d.intensity;
    }
    return min;
}
dane maxVals(const std::vector<dane>& dataset) {
    dane max = dataset[0];  // Инициализация начальным значением
    for (const auto& d : dataset) {
        if (d.x > max.x) max.x = d.x;
        if (d.y > max.y) max.y = d.y;
        if (d.z > max.z) max.z = d.z;
        if (d.intensity > max.intensity) max.intensity = d.intensity;
    }
    return max;
}

std::vector<dane> parseData() {
    std::ifstream file("data.txt");
    std::string line;
    std::vector<dane> dataset;

    if (!file) {
        std::cerr << "Unable to open file data.txt";
        return dataset;
    }

    while (getline(file, line)) {
        std::istringstream iss(line);
        dane temp;
        if (!(iss >> temp.sun_id >> temp.x >> temp.y >> temp.z >> temp.intensity)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue;
        }
        dataset.push_back(temp);
    }

    file.close();
    return dataset;
}

void projectOntoPlanes(const std::vector<dane>& dataset, std::vector<dane>& xy_positive, std::vector<dane>& xy_negative,
    std::vector<dane>& xz_positive, std::vector<dane>& xz_negative,
    std::vector<dane>& yz_positive, std::vector<dane>& yz_negative) {

    for (const auto& data : dataset) {
        double abs_x = std::abs(data.x);
        double abs_y = std::abs(data.y);
        double abs_z = std::abs(data.z);

        if (abs_x > abs_y && abs_x > abs_z) {
            if (data.x >= 0) {
                yz_positive.push_back(data); // x >= 0
            }
            else {
                yz_negative.push_back(data); // x < 0
            }
        }
        else if (abs_y > abs_x && abs_y > abs_z) {
            if (data.y >= 0) {
                xz_positive.push_back(data); // y >= 0
            }
            else {
                xz_negative.push_back(data); // y < 0
            }
        }
        else {
            if (data.z >= 0) {
                xy_positive.push_back(data); // z >= 0
            }
            else {
                xy_negative.push_back(data); // z < 0
            }
        }
    }
}

void generateBitmapSegment(std::vector<std::vector<RGB>>& image, int width, int startY, int endY, const std::unordered_set<Point>& starSet) {
    RGB whitePixel = { 255, 255, 255 };
    RGB blackPixel = { 0, 0, 0 };

    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < width; ++x) {
            Point point = { x, y };
            if (starSet.find(point) != starSet.end()) {
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

    int rowStride = (width * 3 + 3) & ~3;
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowStride * width;

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
        file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

        for (int y = 0; y < width; ++y) {
            for (int x = 0; x < width; ++x) {
                file.write(reinterpret_cast<const char*>(&image[y][x]), sizeof(RGB));
            }
            uint8_t padding[3] = { 0, 0, 0 };
            file.write(reinterpret_cast<const char*>(padding), rowStride - width * 3);
        }

        file.close();
        std::cout << "Bitmap saved as: " << fileName << std::endl;
    }
    else {
        std::cerr << "Cannot create file!" << std::endl;
    }
}

void generateStars(int width, std::unordered_set<Point>& starSet, const std::vector<dane>& dataset, const dane& max, const dane& min) {
    for (const auto& d : dataset) {
        Point star;
        star.x = static_cast<int>(map(d.x, min.x, max.x, 0, width));
        star.y = static_cast<int>(map(d.y, min.y, max.y, 0, width));
        starSet.insert(star);

        int radius = static_cast<int>(map(d.intensity, min.intensity, max.intensity, 1, 3));

        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dy = -radius; dy <= radius; ++dy) {
                if (dx == 0 && dy == 0) continue;
                Point neighbor = { star.x + dx, star.y + dy };
                if (neighbor.x >= 0 && neighbor.x < width && neighbor.y >= 0 && neighbor.y < width) {
                    starSet.insert(neighbor);
                }
            }
        }
    }
}

void generateCubeSides(int width, const std::vector<dane>& xy_positive, const std::vector<dane>& xy_negative,
    const std::vector<dane>& xz_positive, const std::vector<dane>& xz_negative,
    const std::vector<dane>& yz_positive, const std::vector<dane>& yz_negative) {

    std::unordered_set<Point> starSet;
    dane max = maxVals(xy_positive), min = minVals(xy_positive);

    // Side 1: xy_positive
    starSet.clear();
    generateStars(width, starSet, xy_positive, max, min);
    generateBitmap("top.bmp", width, starSet);

    // Side 2: xy_negative
    starSet.clear();
    generateStars(width, starSet, xy_negative, max, min);
    generateBitmap("bottom.bmp", width, starSet);

    // Side 3: xz_positive
    starSet.clear();
    generateStars(width, starSet, xz_positive, max, min);
    generateBitmap("left.bmp", width, starSet);

    // Side 4: xz_negative
    starSet.clear();
    generateStars(width, starSet, xz_negative, max, min);
    generateBitmap("right.bmp", width, starSet);

    // Side 5: yz_positive
    starSet.clear();
    generateStars(width, starSet, yz_positive, max, min);
    generateBitmap("back.bmp", width, starSet);

    // Side 6: yz_negative
    starSet.clear();
    generateStars(width, starSet, yz_negative, max, min);
    generateBitmap("front.bmp", width, starSet);
}

int main() {
    auto dataset = parseData();

    std::vector<dane> xy_positive, xy_negative, xz_positive, xz_negative, yz_positive, yz_negative;
    projectOntoPlanes(dataset, xy_positive, xy_negative, xz_positive, xz_negative, yz_positive, yz_negative);

    int width = 2048;
    generateCubeSides(width, xy_positive, xy_negative, xz_positive, xz_negative, yz_positive, yz_negative);

    return 0;
}
