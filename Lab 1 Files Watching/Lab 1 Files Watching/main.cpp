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
#include <functional>

namespace fs = std::filesystem;

class Signal {
public:
    void connect(std::function<void(bool, uintmax_t, bool)> slot) {
        this->slot = slot;
    }
    
    void emit {
        if (slot)
            slot(exists, size, modified);
    }
    
private:
    std::function<void(bool, uintmax_t, bool)> slot;
};

class FileWatcher {
public:
    FileWatcher(const std::string& path)
        : filePath(path), lastExists(false), lastSize(0)
    {}
    
    Signal fileChanged;
    
    void check() {
        
        bool exists = fs::exists(filePath);
        uintmax_t size = exists ? fs::file_size(filePath) : 0;
        bool modified = false;
        
        if (exists && lastExists && size != lastSize)
            modified = true;
        
        if (exists != lastExists || size != lastSize) {
            lastExists = exists;
            lastSize = size;
            fileChanged.emit(exists, size, modified);
        }
    }
    
private:
    std::string filePath;
    bool lastExists;
    uintmax_t lastSize;
};

void consoleOutput(bool exists, uintmax_t size, bool modified) {
    
    if (!exists)
        std::cout << "Файл не существует\n";
    
    else if (modified)
        std::cout << "Файл существует и был изменён. Размер: " << size << " байт\n";
    
    else
        std::cout << "Файл существует. Размер: " << size << " байт\n";
}

int main() {
    
    std::string path = "test.txt";
    
    FileWatcher watcher(path);
    
    watcher.fileChanged.connect(consoleOutput);
    
    while (true) {
        watcher.check();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}


