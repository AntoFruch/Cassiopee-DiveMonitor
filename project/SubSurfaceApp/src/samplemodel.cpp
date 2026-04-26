#include "samplemodel.h"

void SampleModel::setEntries(const int diveId) {
    // 1. Fetch data from your database
    QList<DiveEntry> entries = DiveDatabase::instance().getDiveEntries(diveId);

    // 2. Convert your custom DiveEntry list into a list of QPointF
    QList<QPointF> points;
    points.reserve(entries.size());

    for (const auto &e : entries) {
        // Here we map: Time -> X axis, Depth -> Y axis
        // Note: Use .toDouble() if time/depth are QVariants
        double x = QVariant(e.time).toDouble();
        double y = QVariant(e.depth).toDouble();

        points.append(QPointF(x, y));
    }

    // 3. Update the series. replace() is faster than clearing and appending 1 by 1.
    this->replace(points);
}