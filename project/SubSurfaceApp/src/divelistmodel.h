#ifndef DIVELISTMODEL_H
#define DIVELISTMODEL_H

#include <QObject>
#include "DiveDataStructure.h"
#include "divedatabase.h"

class DiveListModel
{
    Q_OBJECT

public:
    enum Roles {
        DateRole = Qt::UserRole + 1,
        DiveTimeRole,
        MaxDepthRole,
        AvgDepthRole,
        AtmosPressureRole,
        SurfaceTempRole,
        MinTempRole,
        MaxTempRole,
    };

    DiveListModel(QObject *parent = nullptr);

    void loadDives();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<DiveData> m_dives;

};

#endif // DIVELISTMODEL_H
