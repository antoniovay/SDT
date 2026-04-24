# SDT (Lab 1 FileWatcher)
Software development technology (6 semester)
Отчёт по 1 лабораторной работе «Наблюдение за файлами» в курсе Технология разработки программного обеспечения




------ Постановка задачи ------

Написать программу с консольным интерфейсом, которая выполняет слежение за выбранными файлами.

Ограничимся двумя характеристиками за изменениями которых выполняется слежение:

1. Существование файла;
2. Размер файла.

Программа будет выводить на консоль уведомление о произошедших изменениях в файле.

Существует несколько ситуаций для наблюдаемого файла:

1. Файл существует, файл не пустой - на экран выводится факт существования файла и его размер.
2. Файл существует, файл был изменен - на экран выводится факт существования файла, сообщение о том, что файл был изменен и его размер.  
3. Файл не существует - на экран выводится информация о том, что файл не существует.

При возникновении изменения состояния наблюдаемого файла (возникновение события), необходимо выводить на экран соответствующее сообщение.

В данной реализации используем механизм сигнально-слотового соединения для обеспечения обработки события изменения наблюдаемого файла.

Можно использовать бесконечный цикл, в котором будем обновлять состояние объекта (наблюдаемого файла/файлов) 100 миллисекунд

(например так  std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );).
 
 
 
 
 
 
------ Предлагаемое решение ------

1. Общая идея решения

   Задача решается путём реализации программы, которая выполняет периодическое наблюдение за файлами и фиксирует изменения их состояния.

   Наблюдение осуществляется для двух характеристик:
•    существование файла;
•    размер файла.

   Программа работает в режиме бесконечного цикла, в котором каждые 100 миллисекунд выполняется проверка состояния файловой системы. При изменении состояния файла формируется событие, которое обрабатывается системой уведомлений и выводится в консоль.

2. Общая структура решения

   Решение построено по принципу разделения ответственности и включает следующие основные компоненты:

2.1. Интерфейс обработчика событий

class FileEventRegistrator

   Назначение:
•    определяет интерфейс обработки событий файловой системы;
•    реализует сигнально-слотовый механизм (observer pattern);
•    содержит методы:
    onFileCreated
    onFileModified
    onFileDeleted

   Выступает как “слот” в системе сигналов.

2.2. Класс наблюдателя файлов

class FileWatcher

   Назначение:
•    отвечает за мониторинг одного файла;
•    хранит состояние файла:
    существует ли файл;
    размер файла;
    время последнего изменения;
•    выполняет периодическую проверку состояния файла;
•    генерирует события при изменениях.

   Основные функции:
•    checkFile() — анализ текущего состояния файла;
•    startWatching() — бесконечный цикл наблюдения;
•    addRegistrator() — регистрация обработчиков событий.

   Выступает как “источник сигналов”.

2.3. Реализация обработчика событий

class ConsoleLogger : public FileEventRegistrator

   Назначение:
•    реализует конкретную реакцию на события;
•    выводит информацию о файле в консоль.

   Обрабатывает события:
•    создание файла;
•    изменение файла;
•    удаление файла.

2.4. Главная функция программы

int main()

   Назначение:
•    создание списка наблюдаемых файлов;
•    создание объектов FileWatcher;
•    запуск потоков наблюдения;
•    запуск тестового сценария (создание/изменение/удаление файлов);
•    запуск консольного логгера.

3. Архитектура решения

   Решение построено на основе паттерна проектирования Observer (Наблюдатель).

   Назначение: Observer позволяет организовать механизм уведомлений, при котором:
•    объект-источник (Subject) сообщает о событиях;
•    набор подписчиков (Observers) реагирует на изменения;
•    между ними отсутствует жёсткая связность.

   В данной задаче:
Роль    Роль
Subject (издатель событий)        FileWatcher
Observer (подписчик)    FileEventRegistrator
Concrete Observer    ConsoleLogger

   Преимущества применения паттерна:
•    слабая связность компонентов;
•    возможность добавления новых обработчиков без изменения ядра;
•    расширяемость системы;
•    гибкость реакции на события.

4. UML-описание архитектуры
Текстовая UML-диаграмма:

          Интерфейс
FileEventRegistrator
+onFileCreated()
+onFileModified()
+onFileDeleted()
                   ▲
                    |
ConsoleLogger
+onFileCreated()
+onFileModified()
+onFileDeleted()
                  ▲
                   |
FileWatcher
-filePath
-fileExists
-lastFileSize
-lastWriteTime
-registrators
+checkFile()
+startWatching()
+addRegistrator()

5. Алгоритм работы системы

1.    Программа создаёт набор объектов FileWatcher для каждого файла.
2.    Каждый FileWatcher запускается в отдельном потоке.
3.    В цикле каждые 100 мс выполняется проверка состояния файла.
4.    При изменении состояния генерируется событие:
•    файл появился → onFileCreated
•    файл изменился → onFileModified
•    файл удалён → onFileDeleted
5.    ConsoleLogger получает события и выводит информацию в консоль.

6. Вывод

   Разработанное решение реализует систему наблюдения за файлами на основе:
•    периодического опроса состояния файловой системы;
•    паттерна проектирования Observer;
•    многопоточной обработки наблюдения.

   Система является расширяемой, так как позволяет добавлять новые обработчики событий без изменения логики наблюдения.
 
 
 
 
 
 
------ Код программы ------

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
 Обработчик событий (интерфейс, который описывает, что делать при изменениях файла)
*/
class FileEventRegistrator {
public:
    // появление файла
    virtual void onFileCreated(const fs::path& path, uintmax_t size) = 0;
    
    // изменение файла
    virtual void onFileModified(const fs::path& path, uintmax_t size) = 0;
    
    // удаление файла
    virtual void onFileDeleted(const fs::path& path) = 0;
    
    virtual ~FileEventRegistrator() = default;
};



/*
 Наблюдатель за одним файлом (периодически проверяет состояние файла и вызывает события)
*/
class FileWatcher {
public:
    // инициализация watcher
    FileWatcher(const fs::path& path)
        : filePath(path)
    {
        // проверка существует ли файл при старте
        fileExists = fs::exists(filePath);
        
        // если существует — сохраняем базовые параметры
        if (fileExists) {
            lastFileSize = fs::file_size(filePath);
            lastWriteTime = fs::last_write_time(filePath);
        }
    }
    
    // добавляется обработчик событий (просто ссылка)
    void addRegistrator(FileEventRegistrator* registrator) {
        registrators.push_back(registrator);
    }
    
    // основная функция проверки состояния файла (в цикле)
    void checkFile()
    {
        // текущее состояние файла в системе
        bool existsNow = fs::exists(filePath);

        // файл появился
        if (existsNow && !fileExists)
        {
            fileExists = true;

            uintmax_t size = fs::file_size(filePath);
            auto writeTime = fs::last_write_time(filePath);

            lastFileSize = size;
            lastWriteTime = writeTime;

            // если недавно удалялся - изменение (если файл быстро удалился и появился снова - изменение)
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

        // файл существует (и может измениться)
        else if (existsNow && fileExists)
        {
            uintmax_t newSize = fs::file_size(filePath);
            auto newWriteTime = fs::last_write_time(filePath);

            // проверка реальных изменений
            if (newSize != lastFileSize || newWriteTime != lastWriteTime)
            {
                lastFileSize = newSize;
                lastWriteTime = newWriteTime;

                for (auto r : registrators)
                    r->onFileModified(filePath, newSize);
            }
        }

        // файл удалён
        else if (!existsNow && fileExists)
        {
            fileExists = false;

            // фиксируем время удаления
            lastDeleteTime = std::chrono::steady_clock::now();
            wasRecentlyDeleted = true;

            for (auto r : registrators)
                r->onFileDeleted(filePath);
        }

        //std::cout << "existsNow: " << existsNow << " fileExists: " << fileExists << std::endl;
    }
    
    // запуск наблюдения (бесконечный цикл наблюдения, работает в отдельном потоке)
    void startWatching()
    {
        std::cout << "--Watcher started--\n";
        while (true) {
            checkFile(); // проверка состояния
            
            //пауза 100 мс
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    
private:
    fs::path filePath; // путь до файла
    bool fileExists = false; // существовал ли файл ранее (текущее состояние существования)
    uintmax_t lastFileSize = 0; // последний размер
    fs::file_time_type lastWriteTime; // время последнего изменения
    std::chrono::steady_clock::time_point lastDeleteTime; // время удаления
    bool wasRecentlyDeleted = false; // флаг быстрого удаления/создания
    
    std::vector<FileEventRegistrator*> registrators; // подписчики событий
};



/*
 Консольный логгер (печатает изменения файла)
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

int main() { // создание и тест watcher-ов
    
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
    
        
    // список файлов для наблюдения
    std::vector<fs::path> files = {
        "/Users/antonymiroshnichenko/Desktop/test1.cpp",
        "/Users/antonymiroshnichenko/Desktop/test2.cpp",
        "/Users/antonymiroshnichenko/Desktop/test3.cpp",
        "/Users/antonymiroshnichenko/Desktop/test4.cpp"
    };
    
    // общий логгер
    ConsoleLogger logger;
    
    std::vector<std::unique_ptr<FileWatcher>> watchers; // хранение watcher-ов
    std::vector<std::thread> threads; // потоки наблюдения
    
    // === Запуск watcher'ов ===
    for (const auto& file : files)
    {
        auto watcher = std::make_unique<FileWatcher>(file);
        watcher->addRegistrator(&logger); // подписка на события
        
        threads.emplace_back(&FileWatcher::startWatching, watcher.get()); // запуск в отдельном потоке
        
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
    
    // ждём завершения теста
    testThread.join();
    
    // ждём watcher-ы (они бесконечные)
    for (auto& t : threads)
        t.join();
        
    return 0;
}




 
------ Инструкция пользователя ------

1. Назначение программы

   Данная программа предназначена для наблюдения за файлами в файловой системе и отслеживания их состояния в режиме реального времени.

   Программа контролирует следующие характеристики файлов:
•    существование файла;
•    размер файла.

   При изменении состояния файла программа выводит уведомления в консоль.

2. Подготовка к запуску

   Перед запуском программы необходимо:
1.    Убедиться, что установлена среда разработки C++ (например, Xcode).
2.    Создать или выбрать файлы для наблюдения.
3.    Указать абсолютные пути к файлам в коде программы.

   Пример путей:
/Users/username/Desktop/test1.cpp
/Users/username/Desktop/test2.cpp
/Users/username/Desktop/test3.cpp
/Users/username/Desktop/test4.cpp

3. Запуск программы

   После компиляции программы:
1.    Запустите приложение.
2.    Программа автоматически начнёт наблюдение за указанными файлами.
3.    В консоли появится сообщение о запуске наблюдения:
--Watcher started—

4. Варианты взаимодействия с программой

   Пользователь взаимодействует с программой не напрямую, а через файловую систему.

•    Вариант 1 — Создание файла

   Действие пользователя: 
создаётся файл по указанному пути (если его не было)

   Результат в консоли:
Файл появился: /Users/.../test1.cpp Размер: 0 байт

•    Вариант 2 — Изменение файла

   Действие пользователя:
•    открытие файла в любом редакторе
•    добавление или изменение содержимого
•    сохранение файла

   Результат в консоли:
Файл изменён: /Users/.../test1.cpp Новый размер: 25 байт

•    Вариант 3 — Удаление файла

   Действие пользователя:
удаление файла через Finder или терминал

   Результат в консоли:
Файл удалён: /Users/.../test1.cpp

5. Тестовый сценарий (автоматическая проверка)

   В программе реализован тестовый поток, который автоматически выполняет:
1) Создание файлов
test1.cpp
test2.cpp
test3.cpp
test4.cpp

2) Изменение файлов
   Добавление строки:
Modified

3) Удаление файлов

6. Пример полного вывода программы

--Watcher started--
Файл появился: /Users/.../test1.cpp Размер: 0 байт
Файл изменён: /Users/.../test1.cpp Новый размер: 10 байт
Файл удалён: /Users/.../test1.cpp

7. Особенности использования

•    Программа работает в непрерывном режиме (вечный цикл).
•    Проверка состояния файлов выполняется каждые 100 миллисекунд.
•    Для остановки программы необходимо принудительно завершить процесс (например, Ctrl+C или остановка в IDE).

8. Ограничения

   Отслеживаются только:
•    существование файла;
•    размер файла.
   Не отслеживается содержимое файла напрямую (только через изменение размера и времени записи).
   Не поддерживается рекурсивное наблюдение за папками.

9. Заключение для пользователя

Программа позволяет в реальном времени наблюдать за изменениями файловой системы и получать уведомления о любых изменениях файлов, указанных в конфигурации.





------ Тестирование ------

1. Цель тестирования

   Цель тестирования — проверить корректность работы программы слежения за файлами при различных изменениях их состояния, а также убедиться, что система корректно реагирует на:
•    создание файла;
•    изменение файла;
•    удаление файла;
•    отсутствие файла.

2. Подход к тестированию

   Тестирование проводилось в двух режимах:

1. Ручное тестирование
•    Изменение файлов вручную через редактор (Xcode).
•    Удаление и создание файлов через Finder и терминал.

2. Автоматическое тестирование
•    В программе реализован тестовый поток (testThread), который автоматически:
•    создаёт файлы;
•    изменяет файлы;
•    удаляет файлы.

3. Тестовые файлы

Использовались следующие файлы:
/Users/.../Desktop/test1.cpp
/Users/.../Desktop/test2.cpp
/Users/.../Desktop/test3.cpp
/Users/.../Desktop/test4.cpp

4. Тестовые сценарии (Test Cases)

Case 1 — Файл создаётся (нормальный сценарий)
Условия:
•    файл отсутствует;
•    создаётся новый файл.
Действие:
Создание test1.cpp
Ожидаемый результат:
Файл появился: test1.cpp Размер: 0 байт
Фактический результат:
соответствует ожидаемому

Case 2 — Файл изменяется (нормальный сценарий)
Условия:
•    файл существует;
•    изменяется содержимое файла.
Действие:
Добавление строки
Ожидаемый результат:
Файл изменён: test1.cpp Новый размер: ...
Фактический результат:
корректное определение изменения

Case 3 — Файл удаляется (нормальный сценарий)
Условия:
•    файл существует;
•    файл удаляется вручную или программно.
Действие:
Удаление test1.cpp
Ожидаемый результат:
Файл удалён: test1.cpp
Фактический результат:
корректная обработка удаления

Case 4 — Файл не существует (негативный сценарий)
Условия:
•    файл отсутствует изначально
Действие:
программа запускается
Ожидаемый результат:
Файл не существует (нет событий)
Фактический результат:
событий нет, программа не падает

Case 5 — Неправильный путь к файлу
Условия:
•    указан несуществующий путь
Ожидаемый результат:
•    файл не отслеживается
•    ошибок выполнения нет
Фактический результат:
программа продолжает работу без исключений
Case 6 — Изменение без изменения размера (ограничение системы)
Условия:
•    содержимое файла изменено, но размер совпадает
Ожидаемый результат:
событие может не фиксироваться
Фактический результат:
возможны пропуски при быстрых операциях (частично зависит от last_write_time)



5. Итог тестирования

   По результатам тестирования установлено:
программа корректно обрабатывает основные сценарии:
•    создание файлов;
•    изменение файлов;
•    удаление файлов;

система устойчива к:
•    отсутствующим файлам;
•    неправильным путям;

6. Заключение

Программа прошла функциональное тестирование и корректно выполняет задачи мониторинга файлов в реальном времени в рамках заданных требований.
