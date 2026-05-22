#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

class FileWatcher : public QObject
{
    Q_OBJECT

public:

    // ===== SINGLETON =====
    static FileWatcher& getInstance(const QString& path = "")
    {
        static FileWatcher instance(path);
        return instance;
    }

    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;

signals:

    void fileCreated(const QString& path, qint64 size);
    void fileModified(const QString& path, qint64 size);
    void fileDeleted(const QString& path);

private slots:

    void checkFile();

private:

    explicit FileWatcher(const QString& path, QObject* parent = nullptr)
        : QObject(parent), filePath(path)
    {
        fileInfo.setFile(filePath);

        // начальное состояние
        fileExists = fileInfo.exists();

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

#endif // FILEWATCHER_H
