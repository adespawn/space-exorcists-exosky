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
#include <unordered_map>

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
    int brightness;  // Dodane pole jasności

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
    dane max = dataset[0];
    for (const auto& d : dataset) {
        if (d.x > max.x) max.x = d.x;
        if (d.y > max.y) max.y = d.y;
        if (d.z > max.z) max.z = d.z;
        if (d.intensity > max.intensity) max.intensity = d.intensity;
    }
    return max;
}

// Zastosuj to samo podejście co w pierwszym kodzie
enum CubeFace { XY_POS, XY_NEG, XZ_POS, XZ_NEG, YZ_POS, YZ_NEG };

void generateStars(int width, std::unordered_map<Point, int>& starSet,
    const std::vector<dane>& dataset, const dane& max, const dane& min,
    CubeFace face) {
    for (const auto& d : dataset) {
        Point star;
        // Map coordinates based on the cube face
        switch (face) {
        case XY_POS:
        case XY_NEG:
            star.x = static_cast<int>((d.x + 1.0) * 0.5 * (width - 1));
            star.y = static_cast<int>((d.y + 1.0) * 0.5 * (width - 1));
            break;
        case XZ_POS:
        case XZ_NEG:
            star.x = static_cast<int>((d.x + 1.0) * 0.5 * (width - 1));
            star.y = static_cast<int>((d.z + 1.0) * 0.5 * (width - 1));
            break;
        case YZ_POS:
        case YZ_NEG:
            star.x = static_cast<int>((d.y + 1.0) * 0.5 * (width - 1));
            star.y = static_cast<int>((d.z + 1.0) * 0.5 * (width - 1));
            break;
        }

        int brightness = static_cast<int>(map(d.intensity, min.intensity, max.intensity, 0, 255));
        int radius = static_cast<int>(map(d.intensity, min.intensity, max.intensity, 1, 5));

        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dy = -radius; dy <= radius; ++dy) {
                int distanceSq = dx * dx + dy * dy;
                if (distanceSq <= radius * radius) {
                    Point neighbor = { star.x + dx, star.y + dy };

                    if (neighbor.x >= 0 && neighbor.x < width && neighbor.y >= 0 && neighbor.y < width) {
                        int distance = static_cast<int>(sqrt(distanceSq));
                        int reducedBrightness = brightness * (1.0 - static_cast<double>(distance) / radius);
                        reducedBrightness = std::max(0, reducedBrightness);

                        auto it = starSet.find(neighbor);
                        if (it != starSet.end()) {
                            it->second = std::min(255, it->second + reducedBrightness);
                        }
                        else {
                            starSet[neighbor] = reducedBrightness;
                        }
                    }
                }
            }
        }
    }
}

void generateBitmapSegment(std::vector<std::vector<RGB>>& image, int width, int startY, int endY, const std::unordered_map<Point, int>& starSet) {
    RGB blackPixel = { 0, 0, 0 };

    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < width; ++x) {
            auto it = starSet.find(Point{ x, y });
            if (it != starSet.end()) {
                uint8_t brightness = static_cast<uint8_t>(it->second);  // Jasność punktu
                image[y][x] = { brightness, brightness, brightness };
            }
            else {
                image[y][x] = blackPixel;
            }
        }
    }
}

void generateBitmap(const char* fileName, int width, const std::unordered_map<Point, int>& starSet) {
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    infoHeader.width = width;
    infoHeader.height = width;

    int rowStride = (width * 3 + 3) & ~3;  // Padding do wielokrotności 4 bajtów
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowStride * width;

    // Tworzenie bufora obrazu
    std::vector<std::vector<RGB>> image(width, std::vector<RGB>(width));

    // Ilość wątków na podstawie liczby dostępnych rdzeni
    int numThreads = std::thread::hardware_concurrency();
    int rowsPerThread = width / numThreads;
    std::vector<std::thread> threads;

    // Podziel zadanie na wątki
    for (int i = 0; i < numThreads; ++i) {
        int startY = i * rowsPerThread;
        int endY = (i == numThreads - 1) ? width : startY + rowsPerThread;
        threads.emplace_back(generateBitmapSegment, std::ref(image), width, startY, endY, std::ref(starSet));
    }

    // Oczekiwanie na zakończenie wątków
    for (auto& t : threads) {
        t.join();
    }

    // Zapis obrazu do pliku
    std::ofstream file(fileName, std::ios::binary);
    if (file) {
        file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

        for (int y = 0; y < width; ++y) {
            for (int x = 0; x < width; ++x) {
                file.write(reinterpret_cast<const char*>(&image[y][x]), sizeof(RGB));
            }
            // Dodaj padding do wielokrotności 4 bajtów
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


void generateCubeSides(int width, const std::vector<dane>& xy_positive, const std::vector<dane>& xy_negative,
    const std::vector<dane>& xz_positive, const std::vector<dane>& xz_negative,
    const std::vector<dane>& yz_positive, const std::vector<dane>& yz_negative) {

    std::unordered_map<Point, int> starSet;
    dane max, min;

    // Side 1: xy_positive (top)
    max = maxVals(xy_positive);
    min = minVals(xy_positive);
    starSet.clear();
    generateStars(width, starSet, xy_positive, max, min, XY_POS);
    generateBitmap("top.bmp", width, starSet);

    // Side 2: xy_negative (bottom)
    max = maxVals(xy_negative);
    min = minVals(xy_negative);
    starSet.clear();
    generateStars(width, starSet, xy_negative, max, min, XY_NEG);
    generateBitmap("bottom.bmp", width, starSet);

    // Side 3: xz_positive (front)
    max = maxVals(xz_positive);
    min = minVals(xz_positive);
    starSet.clear();
    generateStars(width, starSet, xz_positive, max, min, XZ_POS);
    generateBitmap("front.bmp", width, starSet);

    // Side 4: xz_negative (back)
    max = maxVals(xz_negative);
    min = minVals(xz_negative);
    starSet.clear();
    generateStars(width, starSet, xz_negative, max, min, XZ_NEG);
    generateBitmap("back.bmp", width, starSet);

    // Side 5: yz_positive (right)
    max = maxVals(yz_positive);
    min = minVals(yz_positive);
    starSet.clear();
    generateStars(width, starSet, yz_positive, max, min, YZ_POS);
    generateBitmap("right.bmp", width, starSet);

    // Side 6: yz_negative (left)
    max = maxVals(yz_negative);
    min = minVals(yz_negative);
    starSet.clear();
    generateStars(width, starSet, yz_negative, max, min, YZ_NEG);
    generateBitmap("left.bmp", width, starSet);
}

void projectOntoPlanes(const std::vector<dane>& dataset,
    std::vector<dane>& xy_positive, std::vector<dane>& xy_negative,
    std::vector<dane>& xz_positive, std::vector<dane>& xz_negative,
    std::vector<dane>& yz_positive, std::vector<dane>& yz_negative) {

    for (const auto& data : dataset) {
        double x = data.x, y = data.y, z = data.z;
        double abs_x = std::abs(x), abs_y = std::abs(y), abs_z = std::abs(z);
        double max_abs = std::max({ abs_x, abs_y, abs_z });

        dane projected = data;

        if (abs_x >= abs_y && abs_x >= abs_z) {
            // X face
            projected.y = y / max_abs;
            projected.z = z / max_abs;
            if (x > 0) {
                projected.x = 1.0;
                yz_positive.push_back(projected);
            }
            else {
                projected.x = -1.0;
                yz_negative.push_back(projected);
            }
        }
        else if (abs_y >= abs_x && abs_y >= abs_z) {
            // Y face
            projected.x = x / max_abs;
            projected.z = z / max_abs;
            if (y > 0) {
                projected.y = 1.0;
                xz_positive.push_back(projected);
            }
            else {
                projected.y = -1.0;
                xz_negative.push_back(projected);
            }
        }
        else {
            // Z face
            projected.x = x / max_abs;
            projected.y = y / max_abs;
            if (z > 0) {
                projected.z = 1.0;
                xy_positive.push_back(projected);
            }
            else {
                projected.z = -1.0;
                xy_negative.push_back(projected);
            }
        }
    }
}

auto randomData() {
	std::vector<dane> dataset;
	for (int i = 0; i < 50000; ++i) {
		dane temp;
		temp.sun_id = i;
		temp.x = static_cast<double>(rand() % 2000)-1000;
		temp.y = static_cast<double>(rand() % 2000)-1000;
		temp.z = static_cast<double>(rand() % 2000)-1000;
		temp.intensity = static_cast<double>(rand() % 2000)-1000;
		dataset.push_back(temp);
	}
	return dataset;
}
int main() {
    auto dataset = parseData();
    //auto dataset = randomData();
        auto projectStart = std::chrono::high_resolution_clock::now();
    std::vector<dane> xy_positive, xy_negative, xz_positive, xz_negative, yz_positive, yz_negative;
    projectOntoPlanes(dataset, xy_positive, xy_negative, xz_positive, xz_negative, yz_positive, yz_negative);
        auto projectEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> projectDuration = projectEnd - projectStart;
        std::cout << "Czas działania projectOntoPlanes: " << projectDuration.count() << " sekund" << std::endl;

        auto sidesStart = std::chrono::high_resolution_clock::now();
    int width = 2048;
    generateCubeSides(width, xy_positive, xy_negative, xz_positive, xz_negative, yz_positive, yz_negative);
        auto sidesEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> sidesDuration = sidesEnd - sidesStart;
        std::cout << "Czas działania generateCubeSides: " << sidesDuration.count() << " sekund" << std::endl;

    return 0;
}
