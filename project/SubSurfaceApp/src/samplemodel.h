#ifndef SAMPLEMODEL_H
#define SAMPLEMODEL_H

#include <QAbstractListModel>
#include "divedatabase.h"
#include "DiveDataStructure.h"

class SampleModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum SampleRoles {
        TimeRole = Qt::UserRole + 1,
        DepthRole,
        TempRole
    };

    explicit SampleModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    void setEntries(const int diveId);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_entries.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    QList<DiveEntry> m_entries;
};

#endif