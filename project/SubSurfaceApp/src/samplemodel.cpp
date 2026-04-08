#include "samplemodel.h"
#include "divecomputerwrapper.h"

void SampleModel::setEntries(const int diveId) {
    beginResetModel();
    m_entries = DiveDatabase::instance().getDiveEntries(diveId);
    endResetModel();
}

QVariant SampleModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_entries.size()) {
        return QVariant();
    }

    const auto &e = m_entries.at(index.row());

    switch (role) {
        case TimeRole:  return e.time;
        case DepthRole: return e.depth;
        case TempRole:  return e.temperature;
        default:        return QVariant();
    }
}

QHash<int, QByteArray> SampleModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TimeRole] = "time";
    roles[DepthRole] = "depth";
    roles[TempRole] = "temp";
    return roles;
}