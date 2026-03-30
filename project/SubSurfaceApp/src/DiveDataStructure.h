#ifndef DIVEDATASTRUCTURE_H
#define DIVEDATASTRUCTURE_H

#include <libdivecomputer/parser.h>
#include <QDateTime>

struct DiveEntry {
    double time;
    QVariant temperature;
    QVariant depth;
};

struct DiveData {
    int id = -1; // auto-incrément DB

    // FINGERPRINT
    QByteArray fingerprint;

    // META DONNEES
    QDateTime date_time;
    unsigned int dive_time;
    double max_depth;
    double avg_depth;
    double atmos_pressure;
    double surface_temperature;
    double min_temperature;
    double max_temperature;

    // DONNEES
    QList<DiveEntry> entries;
};

#endif // DIVEDATASTRUCTURE_H
