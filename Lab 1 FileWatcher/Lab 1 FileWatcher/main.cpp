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
        
        if (fileExists)
            lastFileSize = fs::file_size(filePath);
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
            lastFileSize = fs::file_size(filePath);
            
            for (auto r : registrators)
                r->onFileCreated(filePath, lastFileSize);
        }
        
        //файл существует
        else if (existsNow && fileExists)
        {
            uintmax_t newSize = fs::file_size(filePath);
            
            //файл изменился
            if (newSize != lastFileSize)
            {
                lastFileSize = newSize;
                
                for (auto r : registrators)
                    r->onFileModified(filePath, newSize);
            }
        }
        
        //файл удалён
        else if (!existsNow && fileExists)
        {
            fileExists = false;
            
            for (auto r : registrators)
                r->onFileDeleted(filePath);
        }
        
        std::cout << "existsNow: " << existsNow
                  << " fileExists: " << fileExists << std::endl;
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
    
    return 0;
}
// для тест коммита
