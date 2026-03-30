#include "divelistmodel.h"

DiveListModel::DiveListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void DiveListModel::loadDives()
{
    beginResetModel();
    m_dives = DiveDatabase::instance().getAllDives();
    endResetModel();
}

int DiveListModel::rowCount(const QModelIndex &) const {
    return m_dives.size();
}

QVariant DiveListModel::data(const QModelIndex &index, int role) const
{
    const DiveData &dive = m_dives[index.row()];

    switch (role) {
        case DateRole: return dive.date_time.toString("dd/MM/yyyy");
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

QString DiveListModel::toString() const {
    QString result;
    for (const DiveData &dive : m_dives) {
        result += QString("Date: %1, Temps: %2 min, Profondeur max: %3 m, Profondeur moyenne: %4 m, Pression atmos: %5 bar, Température surface: %6 °C, Température min: %7 °C, Température max: %8 °C\n")
                      .arg(dive.date_time.toString("dd/MM/yyyy"))
                      .arg(dive.dive_time)
                      .arg(dive.max_depth)
                      .arg(dive.avg_depth)
                      .arg(dive.atmos_pressure)
                      .arg(dive.surface_temperature)
                      .arg(dive.min_temperature)
                      .arg(dive.max_temperature);
    }
    return result;
}
