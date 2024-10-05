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
	int brightness;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct dane
{
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

std::vector<dane> parseData() {
    std::ifstream file("data.txt");
    std::string line;
    std::vector<dane> dataset;

    if (!file) {
        std::cerr << "Unable to open file data.txt";
        return dataset;
    }

    // Wczytujemy każdą linię z pliku
    while (getline(file, line))
    {
        std::istringstream iss(line);
        dane temp;

        // Parsujemy wartości z linii i przypisujemy je do odpowiednich pól struktury
        if (!(iss >> temp.sun_id >> temp.x >> temp.y >> temp.z >> temp.intensity)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue; // Pomiń linię w razie błędu
        }

        // Dodajemy dane do wektora
        dataset.push_back(temp);
    }

    file.close();
    return dataset;
}

// Funkcja do generowania fragmentu bitmapy w pamięci
void generateBitmapSegment(std::vector<std::vector<RGB>>& image, int width, int startY, int endY, const std::unordered_set<Point>& starSet) {
    RGB blackPixel = { 0, 0, 0 };

    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isWhite(x, y, starSet)) {
				uint8_t brightness = starSet.find(Point{ x, y })->brightness;
                image[y][x] = { brightness,brightness,brightness };
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

void generateStars(int width, std::unordered_set<Point>& starSet, std::vector<dane> dataset, dane max, dane min) {
    for (auto d : dataset) {
        Point star;
        star.x = map(d.x, min.x, max.x, 0, width);
        star.y = map(d.y, min.y, max.y, 0, width);
        int radius = map(d.intensity, min.intensity, max.intensity, 0, 5);
        int brightness = map(d.intensity, min.intensity, max.intensity, 0, 255);

        // Dodaj centralny piksel (środkowy piksel gwiazdy)
        star.brightness = brightness;
        starSet.insert(star);

        // Dodawanie sąsiadów w obrębie koła
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dy = -radius; dy <= radius; ++dy) {
                // Sprawdź, czy punkt leży w kole o promieniu 'radius'
                if (dx * dx + dy * dy <= radius * radius) {
                    Point neighbor;
                    neighbor.x = star.x + dx;
                    neighbor.y = star.y + dy;
                    neighbor.brightness = brightness;

                    // Sprawdź, czy punkt sąsiadujący mieści się w granicach
                    if (neighbor.x >= 0 && neighbor.x < width && neighbor.y >= 0 && neighbor.y < width) {
                        starSet.insert(neighbor); // Dodaj sąsiada do zbioru
                    }
                }
            }
        }
    }
}


dane maxVals(std::vector<dane> dataset) {
	dane maxValues = { 0, 0, 0, 0, 0 };

	for (auto d : dataset) {
		if (d.x > maxValues.x) maxValues.x = d.x;
		if (d.y > maxValues.y) maxValues.y = d.y;
		if (d.z > maxValues.z) maxValues.z = d.z;
		if (d.intensity > maxValues.intensity) maxValues.intensity = d.intensity;
	}

	return maxValues;
}

dane minVals(std::vector<dane> dataset) {
	dane minValues = { 0, 0, 0, 0, 0 };

	for (auto d : dataset) {
		if (d.x < minValues.x) minValues.x = d.x;
		if (d.y < minValues.y) minValues.y = d.y;
		if (d.z < minValues.z) minValues.z = d.z;
		if (d.intensity < minValues.intensity) minValues.intensity = d.intensity;
	}

	return minValues;
}

int main() {
    const int width = 2048;

    std::vector<dane> dataset = parseData();
    dane maxValues = maxVals(dataset);
	dane minValues = minVals(dataset);

    for (const auto& d : dataset) {
        std::cout << "Sun ID: " << d.sun_id << ", x: " << d.x << ", y: " << d.y << ", z: " << d.z << ", intensity: " << d.intensity << std::endl;
    }
    auto start = std::chrono::high_resolution_clock::now();

    std::unordered_set<Point> starSet;
    generateStars(width, starSet, dataset, maxValues, minValues);

    generateBitmap("output.bmp", width, starSet);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Czas działania: " << duration.count() << " sekund" << std::endl;

#ifdef _WIN32
    system("output.bmp");
#endif

    return 0;
}


/*
TODO:
wybrać odpowiednią ścianę do wyświetlenia
*/