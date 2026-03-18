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

//обработчик событий
class FileEventRegistrator {
public:
    //появление файла
    virtual void fileCreated(const fs::path& path, uintmax_t size) = 0;
    
    //изменение файла
    virtual void fileModified(const fs::path& path, uintmax_t size) = 0;
    
    //удаление файла
    virtual void fileDeleted(const fs::path& path) = 0;
    
    virtual ~FileEventRegistrator() = default;
};



//наблюдатель за файлом
class FileWatcher {
public:
    FileWatcher(const fs::path& path)
        : filePath(path)
    {}
    
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
                r->fileCreated(filePath, lastFileSize);
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
                    r->fileModified(filePath, newSize);
            }
        }
        
        //файл удалён
        else if (!existsNow && fileExists)
        {
            fileExists = false;
            
            for (auto r : registrators)
                r->fileDeleted(filePath);
        }
    }
    
    //запуск наблюдения
    void startWatching()
    {
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



//логгер
class ConsoleLogger : public FileEventRegistrator
{
public:
    
    void fileCreated(const fs::path& path, uintmax_t size) override {
        std::cout << "Файл появился: " << path << " Размер: " << size << " байт\n";
    }
    
    void fileModified(const fs::path& path, uintmax_t size) override {
        std::cout << "Файл изменён: " << path << " Новый размер: " << size << " байт\n";
    }
    
    void fileDeleted(const fs::path& path) override {
        std::cout << "Файл уделён: " << path << std::endl;
    }
};

int main() {
    
    std::cout << "Введите путь к файлу для наблюдения\n";
    
    std::string path;
    std::getline(std::cin, path);
    
    FileWatcher watcher(path);
    
    ConsoleLogger logger;
    
    //соединение наблюдателя с регистрацией
    watcher.addRegistrator(&logger);
    
    std::cout << "Начато наблюдение за файлом\nМожете произвести создание, изменение или удаление файла";
    
    //старт наблюдения
    watcher.startWatching();
    
    return 0;
}
