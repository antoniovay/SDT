//
//  main.cpp
//  Lab 1 Files Watching
//
//  Created by Antony Miroshnichenko on 04.03.2026.
//

#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>
#include <fstream>

namespace fs = std::filesystem;

/*
 Обработчик событий
*/
class FileEventRegistrator {
public:
    //появление файла
    virtual void onFileCreated(const fs::path& path, uintmax_t size) = 0;
    
    //изменение файла
    virtual void onFileModified(const fs::path& path, uintmax_t size) = 0;
    
    //удаление файла
    virtual void onFileDeleted(const fs::path& path) = 0;
    
    virtual ~FileEventRegistrator() = default;
};



/*
 Наблюдатель за файлом
*/
class FileWatcher {
public:
    FileWatcher(const fs::path& path)
        : filePath(path)
    {
        fileExists = fs::exists(filePath);
        
        if (fileExists) {
            lastFileSize = fs::file_size(filePath);
            lastWriteTime = fs::last_write_time(filePath);
        }
    }
    
    //добавляется обработчик событий
    void addRegistrator(FileEventRegistrator* registrator) {
        registrators.push_back(registrator);
    }
    
    //проверка состояния файла
    void checkFile()
    {
        bool existsNow = fs::exists(filePath);

        //файл появился
        if (existsNow && !fileExists)
        {
            fileExists = true;

            uintmax_t size = fs::file_size(filePath);
            auto writeTime = fs::last_write_time(filePath);

            lastFileSize = size;
            lastWriteTime = writeTime;

            //если недавно удалялся - изменение
            if (wasRecentlyDeleted &&
                std::chrono::steady_clock::now() - lastDeleteTime < std::chrono::milliseconds(300))
            {
                for (auto r : registrators)
                    r->onFileModified(filePath, size);

                wasRecentlyDeleted = false;
            }
            else
            {
                for (auto r : registrators)
                    r->onFileCreated(filePath, size);
            }
        }

        //файл существует
        else if (existsNow && fileExists)
        {
            uintmax_t newSize = fs::file_size(filePath);
            auto newWriteTime = fs::last_write_time(filePath);

            if (newSize != lastFileSize || newWriteTime != lastWriteTime)
            {
                lastFileSize = newSize;
                lastWriteTime = newWriteTime;

                for (auto r : registrators)
                    r->onFileModified(filePath, newSize);
            }
        }

        //файл удалён
        else if (!existsNow && fileExists)
        {
            fileExists = false;

            lastDeleteTime = std::chrono::steady_clock::now();
            wasRecentlyDeleted = true;

            for (auto r : registrators)
                r->onFileDeleted(filePath);
        }

        //std::cout << "existsNow: " << existsNow << " fileExists: " << fileExists << std::endl;
    }
    
    //запуск наблюдения
    void startWatching()
    {
        std::cout << "--Watcher started--\n";
        while (true) {
            checkFile();
            
            //пауза 100 мс
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    
private:
    fs::path filePath; //путь до файла
    bool fileExists = false; //существовал ли файл ранее
    uintmax_t lastFileSize = 0; //прошлый размер
    fs::file_time_type lastWriteTime;
    std::chrono::steady_clock::time_point lastDeleteTime;
    bool wasRecentlyDeleted = false;
    
    std::vector<FileEventRegistrator*> registrators;
};



/*
 Консольный логгер
*/
class ConsoleLogger : public FileEventRegistrator
{
public:
    
    void onFileCreated(const fs::path& path, uintmax_t size) override {
        std::cout << "Файл появился: " << path << " Размер: " << size << " байт\n" << std::flush;
    }
    
    void onFileModified(const fs::path& path, uintmax_t size) override {
        std::cout << "Файл изменён: " << path << " Новый размер: " << size << " байт\n" << std::flush;
    }
    
    void onFileDeleted(const fs::path& path) override {
        std::cout << "Файл уделён: " << path << std::endl << std::flush;
    }
};

int main() {
    
/*
    std::cout << "Введите путь к файлу для наблюдения\n";
    
    std::string path;
    std::getline(std::cin, path);
    
    FileWatcher watcher(fs::absolute(path));
    
    ConsoleLogger logger;
    
    //соединение наблюдателя с регистрацией
    watcher.addRegistrator(&logger);
    
    std::cout << "Начато наблюдение за файлом\nМожете произвести создание, изменение или удаление файла" << std::endl;
    
    std::cout << "Путь: " << path << std::endl;
    std::cout << "Существует ли файл: " << fs::exists(path) << std::endl;
    
    //старт наблюдения
    watcher.startWatching();
*/
    
/*
    std::vector<fs::path> files = {
        "/Users/antonymiroshnichenko/Desktop/test1.cpp",
        "/Users/antonymiroshnichenko/Desktop/test2.cpp",
        "/Users/antonymiroshnichenko/Desktop/test3.cpp",
        "/Users/antonymiroshnichenko/Desktop/test4.cpp"
    };

    ConsoleLogger logger;

    std::vector<std::unique_ptr<FileWatcher>> watchers;
    std::vector<std::thread> threads;

    for (const auto& file : files)
    {
        auto watcher = std::make_unique<FileWatcher>(fs::absolute(file));
        watcher->addRegistrator(&logger);
        
        threads.emplace_back(&FileWatcher::startWatching, watcher.get());
        
        watchers.push_back(std::move(watcher));
    }

    // чтобы программа не завершилась
    for (auto& t : threads)
        t.join();
*/
    
        
        
    std::vector<fs::path> files = {
        "/Users/antonymiroshnichenko/Desktop/test1.cpp",
        "/Users/antonymiroshnichenko/Desktop/test2.cpp",
        "/Users/antonymiroshnichenko/Desktop/test3.cpp",
        "/Users/antonymiroshnichenko/Desktop/test4.cpp"
    };
    
    ConsoleLogger logger;
    
    std::vector<std::unique_ptr<FileWatcher>> watchers;
    std::vector<std::thread> threads;
    
    // === Запуск watcher'ов ===
    for (const auto& file : files)
    {
        auto watcher = std::make_unique<FileWatcher>(file);
        watcher->addRegistrator(&logger);
        
        threads.emplace_back(&FileWatcher::startWatching, watcher.get());
        
        watchers.push_back(std::move(watcher));
    }
    
    // === Тестовый поток ===
    std::thread testThread([files]() {
        
        using namespace std::chrono_literals;
        
        std::this_thread::sleep_for(1s);
        
        // === СОЗДАНИЕ ===
        for (const auto& file : files)
        {
            std::ofstream out(file);
            out << "";
            out.close();
        }
        
        std::this_thread::sleep_for(1s);
        
        // === ИЗМЕНЕНИЕ ===
        for (const auto& file : files)
        {
            std::ofstream out(file, std::ios::app);
            out << "Modified\n";
            out.close();
        }
        
        std::this_thread::sleep_for(1s);
        
        // === УДАЛЕНИЕ ===
        for (const auto& file : files)
        {
            fs::remove(file);
        }
        
    });
    
    testThread.join();
    
    for (auto& t : threads)
        t.join();
        
    return 0;
}

