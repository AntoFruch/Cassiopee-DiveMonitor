#ifndef DIVEDATABASE_H
#define DIVEDATABASE_H

#include <QObject>
#include <QSQLDatabase>
#include <QString>
#include <QList>

#include "DiveDataStructure.h"

class DiveDatabase : public QObject {
    Q_OBJECT
public:
    // Accès singleton
    static DiveDatabase& instance(const QString &dbPath = "");
    ~DiveDatabase();

    void insertDive(const DiveData &dive);
    void saveFingerprint(QString vendor, QString product, QByteArray fp);
    QByteArray getFingerprint(QString vendor, QString product);

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
