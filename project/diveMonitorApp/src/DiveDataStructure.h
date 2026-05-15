#ifndef DIVEDATASTRUCTURE_H
#define DIVEDATASTRUCTURE_H

#include <libdivecomputer/parser.h>
#include <QDateTime>

/* ==== STRUCTURES DE DONNÉES POUR RECUPERER DEPUIS OU INSÉRER DANS LA BASE ========== */

/** Une entrée pour un instant temporel
 */
struct DiveEntry {
    double time;            // time stamp (en secondes)
    QVariant temperature;   // température (en °C)
    QVariant depth;         // profondeur (en m)
};

/** Une plongée entière avec son fingerprint, ses meta données et ses entrées.
 */
struct DiveData {
    int id = -1;                // auto-incrément DB

    // FINGERPRINT
    QByteArray fingerprint;     // fingerprint

    // META DONNEES
    QDateTime date_time;        // date
    unsigned int dive_time;     // temps de plongée total
    double max_depth;           // profondeur max
    double avg_depth;           // profondeur moyenne
    double atmos_pressure;      // pression atmosphérique
    double surface_temperature; // température à la surface
    double min_temperature;     // température min
    double max_temperature;     // température max

    // DONNEES
    QList<DiveEntry> entries;   // entrées
};

#endif // DIVEDATASTRUCTURE_H
