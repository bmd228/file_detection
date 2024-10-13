// file_detection.cpp: определяет точку входа для приложения.
//

#include "file_detection.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <magic.h> // Подключаем библиотеку libmagic
#include "embedded_data.h"
#include <filesystem>
#include <windows.h>

using namespace std;
const char hook_string[] = { '?','?','?' };
const string_view emty_string(hook_string, 3);
class FileDetection
{
public:
    FileDetection();
    ~FileDetection();
    std::string_view detectFileTypeByScanning(const std::string& filePath);
private:
    void init();
    magic_t magic= nullptr;
    std::streamsize blockSize = 2048;
    std::vector<char> buffer;
};

FileDetection::FileDetection()
{
    
    buffer.reserve(blockSize);
    buffer.resize(blockSize);
    //buffer.resize()
    try
    {
        init();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
    }
    
}

FileDetection::~FileDetection()
{
        magic_close(magic);
}
void FileDetection::init()
{

    // Используем libmagic для определения типа файла
    magic = magic_open(MAGIC_EXTENSION | MAGIC_MIME| MAGIC_CONTINUE);
    if (magic == nullptr) {
        throw std::runtime_error(reinterpret_cast<const char*>(u8"Не удалось инициализировать libmagic"));
    }
    size_t count = sizeof(embedded::magic) / sizeof(embedded::magic[0]);

    void* data_buffer = (void*)embedded::magic; // Приведение к void*
    void* data_buffers[] = { data_buffer };
    // Загружаем базу данных типов файлов
    if (magic_load_buffers(magic, data_buffers, &count, 1) != 0) {
        magic_close(magic);
        throw std::runtime_error(magic_error(magic));
    }
}
std::string_view FileDetection::detectFileTypeByScanning(const std::string& filePath) {
    // Открываем файл

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(reinterpret_cast<const char*>(u8"Не удалось открыть файл"));
    }
    // Читаем файл блоками и пытаемся распознать тип
    
    std::streamsize offset = 0;
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        std::streamsize bytesRead = file.gcount();

        // Определяем тип файла по текущему блоку данных
        const char* mime_type = magic_buffer(magic, buffer.data(), bytesRead);
        if (mime_type != nullptr && strcmp(mime_type, "???") != 0) {
            std::string_view detectedType(mime_type);
            if (detectedType != "application/octet-stream") {  // Проверяем, что найден не "неизвестный" тип
                //magic_close(magic);
                return detectedType;
            }
                
        }

        // Перемещаемся к следующему блоку
        offset += bytesRead;
        file.clear();  // Сбрасываем флаг EOF, если достигнут конец файла
    }

    // Освобождаем ресурсы
    //magic_close(magic);

    // Если ничего не найдено, возвращаем "неизвестный тип"
    return "unknown";
}
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    try {
        FileDetection file_detection;
        for (auto const& dir_entry : std::filesystem::directory_iterator{ "D:\\test\\" })
        {
            if (dir_entry.is_regular_file()) {
                std::string filePath = dir_entry.path().string();
                std::string_view fileType = file_detection.detectFileTypeByScanning(filePath);
                std::cout << reinterpret_cast<const char*>(u8"Фаил:") << dir_entry.path() << "\t" << reinterpret_cast<const char*>(u8"Тип файла: ") << fileType << std::endl;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << reinterpret_cast<const char*>(u8"Ошибка: ") << ex.what() << std::endl;
    }
	return 0;
}
