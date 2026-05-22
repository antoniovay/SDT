#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "ConsoleLogger.h"
#include "FileWatcher.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString path =
        "/Users/antonymiroshnichenko/Desktop/test.txt";



    // ===== LOGGER =====
    ConsoleLogger logger;



    // ===== SINGLETON WATCHER =====
    auto& watcher =
        FileWatcher::getInstance(path);



    // ===== SIGNAL-SLOT =====

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