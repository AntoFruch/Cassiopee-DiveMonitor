#include "divelistmodel.h"

void DiveListModel::loadDives()
{
    beginResetModel();
    m_dives = DiveDatabase::instance().getAllDives();
    endResetModel();
}

int DiveListModel::rowCount(const QModelIndex &) const {
    return m_dives.size();
}

QVariant DiveListModel::data(const QModelIndex &index, int role)
{
    const DiveData &dive = m_dives[index.row()];

    switch (role) {
        case DateRole: return dive.date_time;
        case DiveTimeRole: return dive.dive_time;
        case MaxDepthRole: return dive.max_depth;
        case AvgDepthRole: return dive.avg_depth;
        case AtmosPressureRole : return dive.atmos_pressure;
        case SurfaceTempRole : return dive.surface_temperature;
        case MinTempRole : return dive.min_temperature;
        case MaxTempRole : return dive.max_temperature;
    }

    return QVariant();
}

QHash<int, QByteArray> DiveListModel::roleNames() const {
    return {
        {DateRole, "date"},
        {DiveTimeRole, "diveTime"},
        {MaxDepthRole, "maxDepth"},
        {AvgDepthRole, "avgDepth"},
        {AtmosPressureRole, "atmosPressure"},
        {SurfaceTempRole, "surfaceTemp"},
        {MinTempRole, "minTemp"},
        {MaxTempRole, "maxTemp"}
    };
}
