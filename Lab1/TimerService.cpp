#include "TimerService.h"

TimerService::TimerService(
    int interval,
    QObject* parent)
    : QObject(parent)
{
    connect(
        &timer,
        &QTimer::timeout,
        this,
        &TimerService::tick);

    timer.start(interval);
}