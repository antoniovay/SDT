#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

class ConsoleLogger : public QObject
{
    Q_OBJECT

public slots:

    void onFileCreated(const QString& path, qint64 size);
    void onFileModified(const QString& path, qint64 size);
    void onFileDeleted(const QString& path);
};

#endif // CONSOLELOGGER_H
