#include <QCoreApplication>
#include <QObject>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include <vector>

/*
    Обработчик событий через механизм сигналов/слотов Qt
*/
class ConsoleLogger : public QObject
{
    Q_OBJECT

public slots:

    // файл создан
    void onFileCreated(const QString& path, qint64 size)
    {
        qDebug() << "Файл появился:" << path
                 << "Размер:" << size << "байт";
    }

    // файл изменён
    void onFileModified(const QString& path, qint64 size)
    {
        qDebug() << "Файл изменён:" << path
                 << "Новый размер:" << size << "байт";
    }

    // файл удалён
    void onFileDeleted(const QString& path)
    {
        qDebug() << "Файл удалён:" << path;
    }
};



/*
    Наблюдатель за файлом через QFileSystemWatcher
*/
class FileWatcher : public QObject
{
    Q_OBJECT

public:

    explicit FileWatcher(const QString& path, QObject* parent = nullptr)
        : QObject(parent),
        filePath(path)
    {
        fileInfo.setFile(filePath);

        // начальное состояние файла
        fileExists = fileInfo.exists();

        if (fileExists)
        {
            lastSize = fileInfo.size();
            lastModified = fileInfo.lastModified();
        }

        // подключение watcher
        watcher.addPath(filePath);

        // сигнал изменения файла
        connect(&watcher,
                &QFileSystemWatcher::fileChanged,
                this,
                &FileWatcher::checkFile);
    }

signals:

    // сигналы событий
    void fileCreated(const QString& path, qint64 size);

    void fileModified(const QString& path, qint64 size);

    void fileDeleted(const QString& path);

private slots:

    // проверка состояния файла
    void checkFile()
    {
        fileInfo.refresh();

        bool existsNow = fileInfo.exists();

        // ===== ФАЙЛ ПОЯВИЛСЯ =====
        if (existsNow && !fileExists)
        {
            fileExists = true;

            lastSize = fileInfo.size();
            lastModified = fileInfo.lastModified();

            emit fileCreated(filePath, lastSize);

            // watcher нужно добавить заново
            watcher.addPath(filePath);
        }

        // ===== ФАЙЛ ИЗМЕНЁН =====
        else if (existsNow && fileExists)
        {
            qint64 newSize = fileInfo.size();
            QDateTime newModified = fileInfo.lastModified();

            if (newSize != lastSize ||
                newModified != lastModified)
            {
                lastSize = newSize;
                lastModified = newModified;

                emit fileModified(filePath, newSize);
            }

            // после изменения Qt иногда удаляет путь
            if (!watcher.files().contains(filePath))
                watcher.addPath(filePath);
        }

        // ===== ФАЙЛ УДАЛЁН =====
        else if (!existsNow && fileExists)
        {
            fileExists = false;

            emit fileDeleted(filePath);
        }
    }

private:

    QString filePath;

    QFileSystemWatcher watcher;

    QFileInfo fileInfo;

    bool fileExists = false;

    qint64 lastSize = 0;

    QDateTime lastModified;
};



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // список файлов
    std::vector<QString> files =
        {
            "/Users/antonymiroshnichenko/Desktop/test1.cpp",
            "/Users/antonymiroshnichenko/Desktop/test2.cpp",
            "/Users/antonymiroshnichenko/Desktop/test3.cpp",
            "/Users/antonymiroshnichenko/Desktop/test4.cpp"
        };

    ConsoleLogger logger;

    std::vector<FileWatcher*> watchers;

    // ===== СОЗДАНИЕ WATCHER-ОВ =====
    for (const auto& file : files)
    {
        auto* watcher = new FileWatcher(file);

        // сигнально-слотовые соединения
        QObject::connect(watcher,
                         &FileWatcher::fileCreated,
                         &logger,
                         &ConsoleLogger::onFileCreated);

        QObject::connect(watcher,
                         &FileWatcher::fileModified,
                         &logger,
                         &ConsoleLogger::onFileModified);

        QObject::connect(watcher,
                         &FileWatcher::fileDeleted,
                         &logger,
                         &ConsoleLogger::onFileDeleted);

        watchers.push_back(watcher);
    }

    // ===== ТЕСТ =====
    QTimer::singleShot(1000, [files]()
                       {
                           // СОЗДАНИЕ
                           for (const auto& file : files)
                           {
                               QFile f(file);

                               if (f.open(QIODevice::WriteOnly))
                               {
                                   f.close();
                               }
                           }
                       });

    QTimer::singleShot(2000, [files]()
                       {
                           // ИЗМЕНЕНИЕ
                           for (const auto& file : files)
                           {
                               QFile f(file);

                               if (f.open(QIODevice::Append))
                               {
                                   QTextStream out(&f);
                                   out << "Modified\n";

                                   f.close();
                               }
                           }
                       });

    QTimer::singleShot(3000, [files]()
                       {
                           // УДАЛЕНИЕ
                           for (const auto& file : files)
                           {
                               QFile::remove(file);
                           }
                       });

    return app.exec();
}

#include "main.moc"