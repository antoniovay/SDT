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

    /*
        Проверка состояния файла
    */
    void checkFile()
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
        }

        // ===== ИЗМЕНЕНИЕ =====
        else if (existsNow && fileExists)
        {
            qint64 newSize = fileInfo.size();

            QDateTime newModified =
                fileInfo.lastModified();

            // проверка реальных изменений
            if (newSize != lastSize ||
                newModified != lastModified)
            {
                lastSize = newSize;
                lastModified = newModified;

                emit fileModified(filePath, newSize);
            }
        }

        // ===== УДАЛЕНИЕ =====
        else if (!existsNow && fileExists)
        {
            fileExists = false;

            emit fileDeleted(filePath);
        }
    }

private:

    // ===== ПРИВАТНЫЙ КОНСТРУКТОР =====
    explicit FileWatcher(const QString& path,
                         QObject* parent = nullptr)
        : QObject(parent), filePath(path)
    {
        fileInfo.setFile(filePath);

        // начальное состояние
        fileExists = fileInfo.exists();

        // если файл существует
        if (fileExists)
        {
            lastSize = fileInfo.size();
            lastModified = fileInfo.lastModified();
        }

        connect(&timer,
                &QTimer::timeout,
                this,
                &FileWatcher::checkFile);


        timer.start(100);
    }

private:

    QString filePath;

    QFileInfo fileInfo;

    bool fileExists = false;

    qint64 lastSize = 0;

    QDateTime lastModified;

    QTimer timer;
};



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ConsoleLogger logger;

    QString path =
        "/Users/antonymiroshnichenko/Desktop/test.txt";

    auto& watcher =
        FileWatcher::getInstance(path);

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