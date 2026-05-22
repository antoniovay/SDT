#include "FileWatcher.h"

void FileWatcher::checkFile()
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