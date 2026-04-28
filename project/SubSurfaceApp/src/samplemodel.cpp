#include "samplemodel.h"

void SampleModel::setEntries(const int diveId) {
    // 1. On charge TOUTES les données une seule fois
    m_rawEntries = DiveDatabase::instance().getDiveEntries(diveId);

    // 2. On affiche selon le mode par défaut
    updateSeries();
}

void SampleModel::setDisplayMode(DisplayMode mode) {
    if (m_currentMode == mode) return;

    m_currentMode = mode;
    updateSeries(); // On re-génère les points sans re-interroger la BDD
}

void SampleModel::updateSeries() {
    QList<QPointF> points;
    points.reserve(m_rawEntries.size());

    for (const auto &e : m_rawEntries) {
        double x = QVariant(e.time).toDouble();
        double y;

        switch (m_currentMode){
            case TEMPERATURE:
                y = QVariant(e.temperature).toDouble();
                break;
            case DEPTH:
                // Depth est inversé pour que la surface soit vers le haut
                y = -QVariant(e.depth).toDouble();
                break;
            case SPEED:
                break;
            default:
                break;
            }

        points.append(QPointF(x, y));
    }

    this->replace(points);
}
