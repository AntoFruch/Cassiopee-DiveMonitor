#ifndef SAMPLEMODEL_H
#define SAMPLEMODEL_H

#include <QtGraphs/QLineSeries>
#include "divedatabase.h"
#include "DiveDataStructure.h"

enum DisplayMode{
    DEPTH = 0,
    SPEED = 1,
    TEMPERATURE = 2,
};

// Inheriting from QLineSeries (or QScatterSeries) allows direct use in GraphsView
class SampleModel : public QLineSeries {
    Q_OBJECT
    Q_PROPERTY(qreal displayMin READ displayMin NOTIFY displayRangeChanged)
    Q_PROPERTY(qreal displayMax READ displayMax NOTIFY displayRangeChanged)
public:
    explicit SampleModel(QObject *parent = nullptr) : QLineSeries(parent), m_currentMode(DEPTH) {} //pour bien initialiser current_mode

    // Method to populate the series with data
    Q_INVOKABLE void setEntries(const int diveId);
    Q_INVOKABLE void setDisplayMode(DisplayMode mode);
    Q_INVOKABLE void updateSeries();

    qreal displayMin() const { return m_displayMin; }
    qreal displayMax() const { return m_displayMax; }

signals:
    void displayRangeChanged();

private:
    void updateDisplayRange(const QList<QPointF> &points);

    DisplayMode m_currentMode;
    QList<DiveEntry> m_rawEntries;
    qreal m_displayMin = -1.0;
    qreal m_displayMax = 0.0;

};

#endif
