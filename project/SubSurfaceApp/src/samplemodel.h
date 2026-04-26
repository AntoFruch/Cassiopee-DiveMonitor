#ifndef SAMPLEMODEL_H
#define SAMPLEMODEL_H

#include <QtGraphs/QLineSeries>
#include "divedatabase.h"
#include "DiveDataStructure.h"

// Inheriting from QLineSeries (or QScatterSeries) allows direct use in GraphsView
class SampleModel : public QLineSeries {
    Q_OBJECT
public:
    explicit SampleModel(QObject *parent = nullptr) : QLineSeries(parent) {}

    // Method to populate the series with data
    Q_INVOKABLE void setEntries(const int diveId);

    // If you still need to calculate velocity specifically for a UI label
    // outside of the graph, you can add a helper function here.
};

#endif