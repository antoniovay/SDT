#include "ConsoleLogger.h"

void ConsoleLogger::onFileCreated(const QString& path, qint64 size)
{
    qDebug() << "Файл появился:"
             << path
             << "Размер:"
             << size;
}

void ConsoleLogger::onFileModified(const QString& path, qint64 size)
{
    qDebug() << "Файл изменён:"
             << path
             << "Размер:"
             << size;
}

void ConsoleLogger::onFileDeleted(const QString& path)
{
    qDebug() << "Файл удалён:"
             << path;
}