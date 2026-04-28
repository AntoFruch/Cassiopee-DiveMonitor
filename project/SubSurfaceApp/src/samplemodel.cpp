#include "samplemodel.h"
#include <algorithm>
#include <cmath>

void SampleModel::updateDisplayRange(const QList<QPointF> &points) {
    qreal minValue = 0.0;
    qreal maxValue = 0.0;

    if (points.isEmpty()) {
        minValue = m_currentMode == DEPTH ? -1.0 : 0.0;
        maxValue = 1.0;
    } else {
        minValue = points.first().y();
        maxValue = points.first().y();

        for (const auto &point : points) {
            minValue = std::min(minValue, point.y());
            maxValue = std::max(maxValue, point.y());
        }

        if (qFuzzyCompare(minValue + 1.0, maxValue + 1.0)) {
            const qreal padding = std::max<qreal>(1.0, std::abs(minValue) * 0.1);
            minValue -= padding;
            maxValue += padding;
        }
    }

    if (qFuzzyCompare(m_displayMin + 1.0, minValue + 1.0)
        && qFuzzyCompare(m_displayMax + 1.0, maxValue + 1.0)) {
        return;
    }

    m_displayMin = minValue;
    m_displayMax = maxValue;
    emit displayRangeChanged();
}

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
    double previousTime = 0.0;
    double previousDepth = 0.0;
    bool hasPreviousDepth = false;

    for (const auto &e : m_rawEntries) {
        double x = QVariant(e.time).toDouble();
        double y = 0.0;

        switch (m_currentMode){
            case TEMPERATURE:
                y = QVariant(e.temperature).toDouble();
                break;
            case DEPTH:
                // Depth est inversé pour que la surface soit vers le haut
                y = -QVariant(e.depth).toDouble();
                break;
            case SPEED: {
                const double currentDepth = QVariant(e.depth).toDouble();

                if (hasPreviousDepth) {
                    const double deltaTime = x - previousTime;

                    if (deltaTime > 0.0)
                        y = std::abs(currentDepth - previousDepth) / deltaTime * 60.0;
                }

                previousTime = x;
                previousDepth = currentDepth;
                hasPreviousDepth = true;
                break;
            }
            default:
                break;
            }

        points.append(QPointF(x, y));
    }

    this->replace(points);
    updateDisplayRange(points);
}
