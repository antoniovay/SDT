#ifndef TIMERSERVICE_H
#define TIMERSERVICE_H

#include <QObject>
#include <QTimer>

class TimerService : public QObject
{
    Q_OBJECT

public:

    explicit TimerService(
        int interval = 100,
        QObject* parent = nullptr);

signals:

    void tick();

private:

    QTimer timer;
};

#endif // TIMERSERVICE_H
