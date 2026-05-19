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

    explicit FileWatcher(const QString& path,
                         QObject* parent = nullptr)
        : QObject(parent),
        filePath(path)
    {
        fileInfo.setFile(filePath);

        dirPath = fileInfo.absolutePath();

        // ВСЕГДА следим за директорией
        watcher.addPath(dirPath);

        // если файл существует — следим и за ним
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

signals:

    void fileCreated(const QString& path, qint64 size);

    void fileModified(const QString& path, qint64 size);

    void fileDeleted(const QString& path);

private slots:

    // изменение директории
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

            // начинаем следить за файлом
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

    // изменение файла
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

        // macOS/Qt иногда удаляет watcher после изменения
        if (!watcher.files().contains(filePath))
            watcher.addPath(filePath);
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

    QString path =
        "/Users/antonymiroshnichenko/Desktop/test.txt";

    FileWatcher watcher(path);

    ConsoleLogger logger;

    QObject::connect(&watcher,
                     &FileWatcher::fileCreated,
                     &logger,
                     &ConsoleLogger::onFileCreated);

    QObject::connect(&watcher,
                     &FileWatcher::fileModified,
                     &logger,
                     &ConsoleLogger::onFileModified);

    QObject::connect(&watcher,
                     &FileWatcher::fileDeleted,
                     &logger,
                     &ConsoleLogger::onFileDeleted);



    // ===== ТЕСТ =====

    // создание
    QTimer::singleShot(1000, [path]()
                       {
                           QFile f(path);

                           if (f.open(QIODevice::WriteOnly))
                           {
                               f.close();
                           }
                       });

    // изменение
    QTimer::singleShot(2000, [path]()
                       {
                           QFile f(path);

                           if (f.open(QIODevice::Append))
                           {
                               QTextStream out(&f);

                               out << "Modified\n";

                               f.close();
                           }
                       });

    // удаление
    QTimer::singleShot(3000, [path]()
                       {
                           QFile::remove(path);
                       });

    return app.exec();
}

#include "main.moc"