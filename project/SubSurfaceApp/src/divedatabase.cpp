#include "divedatabase.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DiveDatabase& DiveDatabase::instance(const QString &dbPath)
{
    static DiveDatabase instance(dbPath);
    return instance;
}

DiveDatabase::DiveDatabase(const QString &dbPath, QObject *parent)
    : QObject(parent), m_dbPath(dbPath)
{
    initDatabase();
}

DiveDatabase::~DiveDatabase()
{
    if (m_db.isOpen())
        m_db.close();
}

void DiveDatabase::initDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qWarning() << "Impossible d'ouvrir la DB:" << m_db.lastError().text();
        return;
    }

    QSqlQuery query(m_db);

    // Création de la table principale des plongées
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS dives ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "fingerprint BLOB, "
            "dive_time INTEGER, "
            "max_depth REAL, "
            "avg_depth REAL, "
            "atmos_pressure REAL, "
            "surface_temperature REAL, "
            "min_temperature REAL, "
            "max_temperature REAL"
            ");"
            )) {
        qWarning() << "Erreur création table dives:" << query.lastError().text();
    }

    // Création de la table des points de plongée
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS dive_entries ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "dive_id INTEGER NOT NULL, "
            "time REAL, "
            "depth REAL, "
            "temperature REAL, "
            "FOREIGN KEY(dive_id) REFERENCES dives(id) ON DELETE CASCADE "
            ");"
            )) {
        qWarning() << "Erreur création table dive_entries:" << query.lastError().text();
    }
}

bool DiveDatabase::insertDive(const DiveData &dive)
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery query(m_db);
    // Insère la plongée
    query.prepare("INSERT INTO dives ("
                    "id,"
                    "fingerprint,"
                    "dive_time,"
                    "max_depth,"
                    "avg_depth,"
                    "atmos_pressure,"
                    "surface_temperature,"
                    "min_temperature,"
                    "max_temperature)"
                "VALUES ("
                    ":id,"
                    ":fingerprint,"
                    ":dive_time,"
                    ":max_depth,"
                    ":avg_depth,"
                    ":atmos_pressure,"
                    ":surface_temperature,"
                    ":min_temperature,"
                    ":max_temperature)"
                  );

    query.bindValue(":fingerprint", dive.fingerprint);
    query.bindValue(":dive_time", dive.dive_time);
    query.bindValue(":max_depth", dive.max_depth);
    query.bindValue(":avg_depth", dive.avg_depth);
    query.bindValue(":atmos_pressure", dive.atmos_pressure);
    query.bindValue(":surface_temperature", dive.surface_temperature);
    query.bindValue(":min_temperature", dive.min_temperature);
    query.bindValue(":max_temperature", dive.max_temperature);

    if (!query.exec()) {
        qWarning() << "Erreur insertion dive:" << query.lastError().text();
        return false;
    }

    // Récupère l'id auto-incrémenté
    int diveId = query.lastInsertId().toInt();

    // Insère les points de plongée
    query.prepare("INSERT INTO dive_entries (dive_id, time, depth, temperature) "
                  "VALUES (:dive_id, :time, :depth, :temperature)");

    for (const auto &entry : dive.entries) {
        query.bindValue(":dive_id", diveId);
        query.bindValue(":time", entry.time);
        query.bindValue(":depth", entry.depth);
        query.bindValue(":temperature", entry.temperature);

        if (!query.exec()) {
            qWarning() << "Erreur insertion dive_entry:" << query.lastError().text();
            return false;
        }
    }

    return true;
}
