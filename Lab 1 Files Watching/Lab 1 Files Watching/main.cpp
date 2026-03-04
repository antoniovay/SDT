//
//  main.cpp
//  Lab 1 Files Watching
//
//  Created by Antony Miroshnichenko on 04.03.2026.
//

#include <iostream>
#include <filesystem>
#include <chrono>

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
    
private:
    std::string filePath;
    bool lastExists;
    uintmax_t lastSize;
};

int main() {
    
}
