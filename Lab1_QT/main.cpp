#include <QCoreApplication>
#include <QObject>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QTextStream>



class ConsoleLogger : public QObject
{
    Q_OBJECT

public slots:

    void onFileCreated(const QString& path, qint64 size)
    {
        qDebug() << "Файл появился:" << path
                 << "Размер:" << size;
    }

    void onFileModified(const QString& path, qint64 size)
    {
        qDebug() << "Файл изменён:" << path
                 << "Размер:" << size;
    }

    void onFileDeleted(const QString& path)
    {
        qDebug() << "Файл удалён:" << path;
    }
};



class FileWatcher : public QObject
{
    Q_OBJECT

public:

    // ===== ДОСТУП К ЕДИНСТВЕННОМУ ЭКЗЕМПЛЯРУ =====
    static FileWatcher& getInstance(const QString& path = "")
    {
        static FileWatcher instance(path);

        return instance;
    }

    // запрет копирования
    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;

signals:

    void fileCreated(const QString& path, qint64 size);

    void fileModified(const QString& path, qint64 size);

    void fileDeleted(const QString& path);

private slots:

    void onDirectoryChanged(const QString&)
    {
        fileInfo.refresh();

        bool existsNow = fileInfo.exists();

        // ===== СОЗДАНИЕ =====
        if (existsNow && !fileExists)
        {
            fileExists = true;

            lastSize = fileInfo.size();
            lastModified = fileInfo.lastModified();

            emit fileCreated(filePath, lastSize);

            if (!watcher.files().contains(filePath))
                watcher.addPath(filePath);
        }

        // ===== УДАЛЕНИЕ =====
        else if (!existsNow && fileExists)
        {
            fileExists = false;

            emit fileDeleted(filePath);
        }
    }

    void onFileChanged(const QString&)
    {
        fileInfo.refresh();

        if (!fileInfo.exists())
            return;

        qint64 newSize = fileInfo.size();

        QDateTime newModified = fileInfo.lastModified();

        if (newSize != lastSize ||
            newModified != lastModified)
        {
            lastSize = newSize;
            lastModified = newModified;

            emit fileModified(filePath, newSize);
        }

        // восстановление watcher
        if (!watcher.files().contains(filePath))
            watcher.addPath(filePath);
    }

private:

    // ===== ПРИВАТНЫЙ КОНСТРУКТОР =====
    explicit FileWatcher(const QString& path,
                         QObject* parent = nullptr)
        : QObject(parent),
        filePath(path)
    {
        fileInfo.setFile(filePath);

        dirPath = fileInfo.absolutePath();

        // следим за директорией
        watcher.addPath(dirPath);

        // если файл существует
        if (fileInfo.exists())
        {
            fileExists = true;

            lastSize = fileInfo.size();
            lastModified = fileInfo.lastModified();

            watcher.addPath(filePath);
        }

        connect(&watcher,
                &QFileSystemWatcher::directoryChanged,
                this,
                &FileWatcher::onDirectoryChanged);

        connect(&watcher,
                &QFileSystemWatcher::fileChanged,
                this,
                &FileWatcher::onFileChanged);
    }

private:

    QString filePath;

    QString dirPath;

    QFileInfo fileInfo;

    QFileSystemWatcher watcher;

    bool fileExists = false;

    qint64 lastSize = 0;

    QDateTime lastModified;
};



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ConsoleLogger logger;

    auto& watcher =
        FileWatcher::getInstance(
            "/Users/antonymiroshnichenko/Desktop/test.txt"
            );

    QObject::connect(
        &watcher,
        &FileWatcher::fileCreated,
        &logger,
        &ConsoleLogger::onFileCreated
        );

    QObject::connect(
        &watcher,
        &FileWatcher::fileModified,
        &logger,
        &ConsoleLogger::onFileModified
        );

    QObject::connect(
        &watcher,
        &FileWatcher::fileDeleted,
        &logger,
        &ConsoleLogger::onFileDeleted
        );

    return app.exec();
}

#include "main.moc"