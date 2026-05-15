#ifndef DIVELISTMODEL_H
#define DIVELISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "DiveDataStructure.h"
#include "divedatabase.h"

/** DiveListModel est un composant QML Custom qui permet de modéliser la liste de l'ensemble des plongée présentes en base.
 *  Chaque plongée contient uniquement les méta données, pas les entrées temporelles.
 *
 */
class DiveListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /** Enum des roles dans la liste.
     *  Pour en ajouter un le rajouter et le rajouter ici, ainsi que dans l'implémentation des méthodes data() et rolrNames()
     */
    enum Roles {
        IdRole = Qt::UserRole+1,
        DateRole,
        DiveTimeRole,
        MaxDepthRole,
        AvgDepthRole,
        AtmosPressureRole,
        SurfaceTempRole,
        MinTempRole,
        MaxTempRole,
    };

    /** Constructeur.
     *
     * @param parent
     */
    explicit DiveListModel(QObject *parent = nullptr);

    /** Charge les plongées dans m_dives de puis la base de données
     *
     */
    Q_INVOKABLE void loadDives();

    /** Donne le nombre de lignes de la structure
     *  /!\ Override d'une méthode abstraite (virtual) de QAbstractList
     *
     * @param parent <- j'ai pas compris cet argument
     * @return le nombre de lignes de la structure.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /** Donne la donnée de la ligne index et du role role.
     *  /!\ Override d'une méthode abstraite (virtual) de QAbstractList
     *
     * @param index
     * @param role
     * @return la donnée.
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /** ToString
     *
     * @return
     */
    QString toString() const;


protected:
    /**
     *  /!\ Override d'une méthode abstraite (virtual) de QAbstractList
     * @return
     */
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<DiveData> m_dives;

};

#endif // DIVELISTMODEL_H
