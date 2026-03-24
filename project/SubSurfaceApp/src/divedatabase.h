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
