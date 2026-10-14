#ifndef SMA_H
#define SMA_H

#include <iostream>
#include <stddef.h>
#include <assert.h>
#include <QObject>

class SMA : public QObject{

    Q_OBJECT

public:
    SMA(unsigned int);
    ~SMA();
    void add(double);
    double avg() const;


private:
    unsigned int period;
    double * window; // Holds the values to calculate the average of.

    // Logically, head is before tail
    double * head; // Points at the oldest element we've stored.
    double * tail; // Points at the newest element we've stored.

    double total; // Cache the total so we don't sum everything each time.
    void inc(double);
    ptrdiff_t size() const;

#endif // SMA_H
