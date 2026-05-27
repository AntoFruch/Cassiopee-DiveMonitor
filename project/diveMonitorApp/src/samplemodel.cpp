#include "samplemodel.h"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>

namespace {
bool hasSampleValue(const QVariant &value)
{
    return value.isValid() && !value.isNull();
}
}

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

QVariantMap SampleModel::sampleDetailsAtRenderCoordinates(qreal renderX, qreal renderY) const
{
    if (m_rawEntries.isEmpty())
        return { { "valid", false } };

    const QPointF queryPoint = const_cast<SampleModel *>(this)->dataPointCoordinatesAt(renderX, renderY);
    return buildSampleDetails(closestSampleIndex(queryPoint));
}

QPointF SampleModel::displayPointAtIndex(int index) const
{
    if (!isValidEntryIndex(index))
        return QPointF();

    return QPointF(m_rawEntries.at(index).time, displayValueForEntry(index));
}

QVariantMap SampleModel::sampleRangeDetails(int firstIndex, int secondIndex) const
{
    QVariantMap details;
    details.insert("valid", false);
    details.insert("isRange", true);

    if (!isValidEntryIndex(firstIndex) || !isValidEntryIndex(secondIndex) || firstIndex == secondIndex)
        return details;

    const int startIndex = std::min(firstIndex, secondIndex);
    const int endIndex = std::max(firstIndex, secondIndex);
    const DiveEntry &startEntry = m_rawEntries.at(startIndex);
    const DiveEntry &endEntry = m_rawEntries.at(endIndex);

    if (!hasSampleValue(startEntry.depth) || !hasSampleValue(endEntry.depth))
        return details;

    const double durationSeconds = endEntry.time - startEntry.time;
    if (durationSeconds <= 0.0)
        return details;

    const double startDepth = startEntry.depth.toDouble();
    const double endDepth = endEntry.depth.toDouble();
    const double deltaDepth = endDepth - startDepth;

    details.insert("valid", true);
    details.insert("startIndex", startIndex);
    details.insert("endIndex", endIndex);
    details.insert("startTimeSeconds", startEntry.time);
    details.insert("endTimeSeconds", endEntry.time);
    details.insert("durationSeconds", durationSeconds);
    details.insert("startDepthMeters", startDepth);
    details.insert("endDepthMeters", endDepth);
    details.insert("deltaDepthMeters", deltaDepth);
    details.insert("averageSpeedMetersPerMinute", (deltaDepth / durationSeconds) * 60.0);
    return details;
}

qreal SampleModel::displayValueForEntry(int index) const
{
    if (!isValidEntryIndex(index))
        return 0.0;

    const DiveEntry &entry = m_rawEntries.at(index);

    switch (m_currentMode) {
    case TEMPERATURE:
        return hasSampleValue(entry.temperature) ? entry.temperature.toDouble() : 0.0;
    case DEPTH:
        // Depth est inversé pour que la surface soit vers le haut.
        return -(hasSampleValue(entry.depth) ? entry.depth.toDouble() : 0.0);
    case SPEED:
        return speedValueForEntry(index);
    default:
        return 0.0;
    }
}

qreal SampleModel::speedValueForEntry(int index) const
{
    if (!isValidEntryIndex(index))
        return 0.0;

    constexpr int regressionRadius = 2;
    const int entryCount = static_cast<int>(m_rawEntries.size());

    const auto sampleAt = [this](int sampleIndex, double &time, double &depth) {
        if (!isValidEntryIndex(sampleIndex))
            return false;

        const DiveEntry &entry = m_rawEntries.at(sampleIndex);
        if (!hasSampleValue(entry.depth))
            return false;

        time = entry.time;
        depth = entry.depth.toDouble();
        return true;
    };

    const auto speedBetween = [](double startTime, double startDepth,
                                 double endTime, double endDepth) {
        const double deltaTime = endTime - startTime;
        if (deltaTime <= 0.0)
            return 0.0;

        return ((endDepth - startDepth) / deltaTime) * 60.0;
    };

    double currentTime = 0.0;
    double currentDepth = 0.0;
    if (!sampleAt(index, currentTime, currentDepth))
        return 0.0;

    const int startIndex = std::max(0, index - regressionRadius);
    const int endIndex = std::min(entryCount - 1, index + regressionRadius);

    double timeSum = 0.0;
    double depthSum = 0.0;
    double sampleCount = 0.0;

    for (int sampleIndex = startIndex; sampleIndex <= endIndex; ++sampleIndex) {
        double sampleTime = 0.0;
        double sampleDepth = 0.0;
        if (sampleAt(sampleIndex, sampleTime, sampleDepth)) {
            timeSum += sampleTime - currentTime;
            depthSum += sampleDepth;
            sampleCount += 1.0;
        }
    }

    if (sampleCount >= 2.0) {
        const double averageTime = timeSum / sampleCount;
        const double averageDepth = depthSum / sampleCount;
        double numerator = 0.0;
        double denominator = 0.0;

        for (int sampleIndex = startIndex; sampleIndex <= endIndex; ++sampleIndex) {
            double sampleTime = 0.0;
            double sampleDepth = 0.0;
            if (!sampleAt(sampleIndex, sampleTime, sampleDepth))
                continue;

            const double centeredTime = (sampleTime - currentTime) - averageTime;
            numerator += centeredTime * (sampleDepth - averageDepth);
            denominator += centeredTime * centeredTime;
        }

        if (denominator > 0.0)
            return (numerator / denominator) * 60.0;
    }

    double neighborTime = 0.0;
    double neighborDepth = 0.0;
    if (index + 1 < entryCount && sampleAt(index + 1, neighborTime, neighborDepth))
        return speedBetween(currentTime, currentDepth, neighborTime, neighborDepth);

    if (index > 0 && sampleAt(index - 1, neighborTime, neighborDepth))
        return speedBetween(neighborTime, neighborDepth, currentTime, currentDepth);

    return 0.0;
}

QVariantMap SampleModel::buildSampleDetails(int index) const
{
    QVariantMap details;
    details.insert("valid", false);

    if (!isValidEntryIndex(index))
        return details;

    const DiveEntry &entry = m_rawEntries.at(index);
    const bool hasTemperature = hasSampleValue(entry.temperature);

    details.insert("valid", true);
    details.insert("index", index);
    details.insert("timeSeconds", entry.time);
    details.insert("depthMeters", hasSampleValue(entry.depth) ? entry.depth.toDouble() : 0.0);
    details.insert("temperatureCelsius", hasTemperature ? entry.temperature.toDouble() : 0.0);
    details.insert("hasTemperature", hasTemperature);
    details.insert("speedMetersPerMinute", speedValueForEntry(index));
    details.insert("displayY", displayValueForEntry(index));
    return details;
}

int SampleModel::closestSampleIndex(const QPointF &queryPoint) const
{
    if (m_rawEntries.isEmpty())
        return -1;

    const auto lowerIt = std::lower_bound(
        m_rawEntries.cbegin(),
        m_rawEntries.cend(),
        queryPoint.x(),
        [](const DiveEntry &entry, qreal value) {
            return entry.time < value;
        }
    );

    QList<int> candidateIndexes;
    if (lowerIt != m_rawEntries.cend())
        candidateIndexes.append(static_cast<int>(std::distance(m_rawEntries.cbegin(), lowerIt)));
    if (lowerIt != m_rawEntries.cbegin())
        candidateIndexes.append(static_cast<int>(std::distance(m_rawEntries.cbegin(), lowerIt)) - 1);

    if (candidateIndexes.isEmpty())
        candidateIndexes.append(m_rawEntries.size() - 1);

    const double xRange = std::max(1.0, m_rawEntries.last().time - m_rawEntries.first().time);
    const double yRange = std::max(1.0, static_cast<double>(m_displayMax - m_displayMin));

    int bestIndex = candidateIndexes.first();
    double bestDistanceSquared = std::numeric_limits<double>::max();

    for (int candidateIndex : candidateIndexes) {
        const double candidateX = m_rawEntries.at(candidateIndex).time;
        const double candidateY = displayValueForEntry(candidateIndex);
        const double dx = (candidateX - queryPoint.x()) / xRange;
        const double dy = (candidateY - queryPoint.y()) / yRange;
        const double distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < bestDistanceSquared) {
            bestDistanceSquared = distanceSquared;
            bestIndex = candidateIndex;
        }
    }

    return bestIndex;
}

bool SampleModel::isValidEntryIndex(int index) const
{
    return index >= 0 && index < m_rawEntries.size();
}

void SampleModel::updateSeries() {
    QList<QPointF> points;
    points.reserve(m_rawEntries.size());
    for (int index = 0; index < m_rawEntries.size(); ++index) {
        const DiveEntry &entry = m_rawEntries.at(index);
        points.append(QPointF(entry.time, displayValueForEntry(index)));
    }

    this->replace(points);
    updateDisplayRange(points);
}
