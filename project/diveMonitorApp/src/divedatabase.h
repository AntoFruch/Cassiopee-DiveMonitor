#ifndef DIVEDATABASE_H
#define DIVEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QList>

#include "DiveDataStructure.h"

class DiveDatabase : public QObject {
    Q_OBJECT
public:

    // Accès singleton
    static DiveDatabase& instance();
    ~DiveDatabase();

    // Insertion en base
    /** Insertion d'un plongée dans la base de donnée
     *
     *  @param dive : reference vers la plongée ( structure DiveData )
     */
    void insertDive(const DiveData &dive);

    /** Update du nouveau fingerprint dans la base de donnée
     *
     *  @param vendor : nom du fabricant de l'appareil
     *  @param product : nom du modèle
     *  @param fp : fingerprint à insérer
     */
    void saveFingerprint(QString vendor, QString product, QByteArray fp);

    // Selection depuis la base
    /** Selection du fingerprint
     *
     *  @param vendor : nom du fabricant de l'appareil
     *  @param product : nom du modèle
     *  @returns le fingerprint voulu s'il existe, un QByteArray vide sinon
     */
    QByteArray getFingerprint(QString vendor, QString product);

    /** recupère toute les plongées en base et les retourne sous forme de liste de DiveData
     *
     * @returns l'ensemble des plongée en base.
     */
    QList<DiveData> getAllDives();

    /** Récupère les données de la plongées (pas les metadonnées)
     *
     *  @param id
     *  @returns les données temporelles de la plongée
     */
    QList<DiveEntry> getDiveEntries(int id);

private:
    // Constructeur privé et interdiction de copie.
    explicit DiveDatabase(const QString &dbPath, QObject *parent = nullptr);
    DiveDatabase(const DiveDatabase&) = delete;
    DiveDatabase& operator=(const DiveDatabase&) = delete;


    QString m_dbPath;
    QSqlDatabase m_db;


    void initDatabase();
};

#endif // DIVEDATABASE_H
