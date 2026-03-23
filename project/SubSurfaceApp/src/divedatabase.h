#ifndef DIVEDATABASE_H
#define DIVEDATABASE_H

#include <QObject>
#include <QSQLDatabase>
#include <QString>
#include <QList>

struct DiveEntry {
    double depth;
    double temperature;
    double time; // temps depuis le début de la plongée
};

struct DiveData {
    int id = -1; // auto-incrément DB

    // FINGERPRINT
    QByteArray fingerprint;

    // META DONNEES
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

class DiveDatabase : public QObject {
    Q_OBJECT
public:
    // Accès singleton
    static DiveDatabase& instance(const QString &dbPath = "");
    ~DiveDatabase();

    bool insertDive(const DiveData &dive);

private:
    explicit DiveDatabase(const QString &dbPath, QObject *parent = nullptr);

    QString m_dbPath;
    QSqlDatabase m_db;

    // interdiction de copie
    DiveDatabase(const DiveDatabase&) = delete;
    DiveDatabase& operator=(const DiveDatabase&) = delete;

    void initDatabase();
};

#endif // DIVEDATABASE_H
