#include "divedatabase.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QDir>
#include <QTime>
#include <QDateTime>
#include <QCoreApplication>
#include <QStandardPaths>

DiveDatabase& DiveDatabase::instance()
{
    // 1. Get the standard path for App Data
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    // 2. Ensure the directory exists (Qt won't create it for you automatically)
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 3. Define the full database path
    QString dbPath = dir.filePath("database.db");

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
            "date_time DATE, "
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

    // Création de l'association appareil / dernier fingerprint
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS fingerprints ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "vendor TEXT COLLATE NOCASE, "
            "product TEXT COLLATE NOCASE, "
            "fingerprint BLOB, "
            "UNIQUE(vendor, product)"
            ");"
            )){
        qWarning() << "Erreur création table fingerprints:" << query.lastError().text();
    }
}

void DiveDatabase::insertDive(const DiveData &dive)
{
    if (!m_db.isOpen())
        return ;

    QSqlQuery query(m_db);
    // Insère la plongée
    query.prepare("INSERT INTO dives ("
                    "fingerprint,"
                    "date_time, "
                    "dive_time,"
                    "max_depth,"
                    "avg_depth,"
                    "atmos_pressure,"
                    "surface_temperature,"
                    "min_temperature,"
                    "max_temperature)"
                "VALUES ("
                    ":fingerprint,"
                    ":date_time,"
                    ":dive_time,"
                    ":max_depth,"
                    ":avg_depth,"
                    ":atmos_pressure,"
                    ":surface_temperature,"
                    ":min_temperature,"
                    ":max_temperature)"
                  );

    query.bindValue(":fingerprint", dive.fingerprint);
    query.bindValue(":date_time", dive.date_time.toString(Qt::ISODate));
    query.bindValue(":dive_time", dive.dive_time);
    query.bindValue(":max_depth", dive.max_depth);
    query.bindValue(":avg_depth", dive.avg_depth);
    query.bindValue(":atmos_pressure", dive.atmos_pressure);
    query.bindValue(":surface_temperature", dive.surface_temperature);
    query.bindValue(":min_temperature", dive.min_temperature);
    query.bindValue(":max_temperature", dive.max_temperature);

    if (!query.exec()) {
        qWarning() << "Erreur insertion dive:" << query.lastError().text();
        return;
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
            return ;
        }
    }

    return ;
}

void DiveDatabase::saveFingerprint(QString vendor, QString product, QByteArray fp){
    if (!m_db.isOpen())
        return ;

    QSqlQuery query(m_db);

    query.prepare(
        "INSERT INTO fingerprints (vendor, product, fingerprint) "
        "VALUES (:vendor, :product, :fingerprint) "
        "ON CONFLICT(vendor, product) DO UPDATE SET fingerprint = :fingerprint"
        );
    query.bindValue(":vendor", vendor);
    query.bindValue(":product", product);
    query.bindValue(":fingerprint", fp);

    if (!query.exec()) {
        qWarning() << "Erreur insertion fingerprints:" << query.lastError().text();
        return ;
    }
    return;
}


QByteArray DiveDatabase::getFingerprint(QString vendor, QString product){
    if (!m_db.isOpen()){
        return QByteArray();
    }

    QSqlQuery query(m_db);

    query.prepare(
        "SELECT fingerprint from fingerprints "
        "WHERE vendor = :vendor AND product = :product"
        );
    query.bindValue(":vendor", vendor);
    query.bindValue(":product", product);

    if (!query.exec()) {
        qWarning() << "Erreur selection fingerprints:" << query.lastError().text();
        return NULL;
    }

    if (query.next()) {
        QByteArray fingerprint = query.value(0).toByteArray(); // Récupération du champ
        qDebug() << "fingerprint selected";
        return fingerprint;
    } else {
        qWarning() << "Aucun fingerprint trouvé pour" << vendor << product;
        return QByteArray(); // Retourner vide si pas de résultat
    }
}

QList<DiveData> DiveDatabase::getAllDives(){

    if (!m_db.isOpen()){
        return QList<DiveData>();
    }

    QList<DiveData> dives;
    QSqlQuery query(m_db);

    query.prepare(
        "SELECT * FROM dives ORDER BY date_time DESC"
        );

    if (!query.exec()) {
        qWarning() << "Erreur selection dives:" << query.lastError().text();
        return QList<DiveData>();
    }

    while (query.next()) {
        DiveData dive;

        dive.id = query.value("id").toInt();
        dive.fingerprint = query.value("fingerprint").toByteArray();
        dive.date_time = query.value("date_time").toDateTime();
        dive.dive_time = query.value("dive_time").toInt();
        dive.max_depth = query.value("max_depth").toDouble();
        dive.avg_depth = query.value("avg_depth").toDouble();
        dive.atmos_pressure = query.value("atmos_pressure").toDouble();
        dive.surface_temperature = query.value("surface_temperature").toDouble();
        dive.min_temperature = query.value("min_temperature").toDouble();
        dive.max_temperature = query.value("max_temperature").toDouble();

        dives.append(dive);
    }
    qDebug() << dives.length() << " dives selected.";
    return dives;
}

QList<DiveEntry> DiveDatabase::getDiveEntries(int id){
    if (!m_db.isOpen()){
        return QList<DiveEntry>();
    }

    QList<DiveEntry> entries;
    QSqlQuery query(m_db);

    qDebug() << "selecting dive" << id;

    query.prepare(
        "SELECT * FROM dive_entries WHERE dive_id=:id ORDER BY time ASC" // pour avoir les points dans le bon ordre temporel
        );

    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "Erreur selection dive_entries:" << query.lastError().text();
        return QList<DiveEntry>();
    }

    while (query.next()){
        DiveEntry entry;
        entry.time = query.value("time").toDouble();
        entry.temperature = query.value("temperature");
        entry.depth = query.value("depth");

        entries.append(entry);
    }
    qDebug() << entries.length() << "points selected";
    return entries;
}
