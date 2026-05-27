#ifndef SAMPLEMODEL_H
#define SAMPLEMODEL_H

#include <QVariantMap>
#include <QtGraphs/QLineSeries>
#include "divedatabase.h"
#include "DiveDataStructure.h"

enum DisplayMode{
    DEPTH = 0,
    SPEED = 1,
    TEMPERATURE = 2,
};

/** Modélise les points du graphe qui seront affichés dans l'interface sur le graphe.
 * Hérite de QLineSeries pour être directement lisible par un GraphView dans le QML.
 *
 * Cette classe est destinée à être utilisée comme un composant QML Custom, donc directement instanciée dans le QML.
 */
class SampleModel : public QLineSeries {
    Q_OBJECT
    Q_PROPERTY(qreal displayMin READ displayMin NOTIFY displayRangeChanged)
    Q_PROPERTY(qreal displayMax READ displayMax NOTIFY displayRangeChanged)
public:
    explicit SampleModel(QObject *parent = nullptr) : QLineSeries(parent), m_currentMode(DEPTH) {} //pour bien initialiser current_mode

    /** Récupère les entrées de la plongée diveId depuis la base de donnée, puis charge m_rawEntries avec
     *  updateSeries() qui prend compte de m_currentMode pour mettre la bonne colonne dans les ordonéees (température, profondeur, etc)
     *
     * @param diveId
     */
    Q_INVOKABLE void setEntries(const int diveId);

    /** setter pour m_currentmode
     *
     * @param mode
     */
    Q_INVOKABLE void setDisplayMode(DisplayMode mode);
    Q_INVOKABLE QVariantMap sampleDetailsAtRenderCoordinates(qreal renderX, qreal renderY) const;
    Q_INVOKABLE QPointF displayPointAtIndex(int index) const;
    Q_INVOKABLE QVariantMap sampleRangeDetails(int firstIndex, int secondIndex) const;

    qreal displayMin() const { return m_displayMin; }
    qreal displayMax() const { return m_displayMax; }

signals:
    void displayRangeChanged();

private:
    void updateDisplayRange(const QList<QPointF> &points);
    qreal displayValueForEntry(int index) const;
    qreal speedValueForEntry(int index) const;
    QVariantMap buildSampleDetails(int index) const;
    int closestSampleIndex(const QPointF &queryPoint) const;
    bool isValidEntryIndex(int index) const;

    DisplayMode m_currentMode;
    QList<DiveEntry> m_rawEntries;
    qreal m_displayMin = -1.0;
    qreal m_displayMax = 0.0;


    void updateSeries();

};

#endif
