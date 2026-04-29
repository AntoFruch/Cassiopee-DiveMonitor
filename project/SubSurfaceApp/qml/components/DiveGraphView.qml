import QtQuick
import QtGraphs
import QtQuick.Layouts
import QtQuick.Controls 2.15
import DiveMonitorCustom 1.0

Item {
    id: root

    required property var dive
    required property color accentColorValue
    required property color backgroundColorValue
    required property color textColorValue

    property bool showModeSelector: true
    property bool overlayModeSelector: false
    property bool showExpandButton: false
    property int displayMode: displayDepth

    readonly property int displayDepth: 0
    readonly property int displaySpeed: 1
    readonly property int displayTemperature: 2
    readonly property var yModeOptions: [
        { label: "Profondeur", mode: displayDepth },
        { label: "Vitesse", mode: displaySpeed },
        { label: "Temperature", mode: displayTemperature }
    ]

    signal expandRequested()

    function modeIndexFor(mode) {
        for (let i = 0; i < yModeOptions.length; ++i) {
            if (yModeOptions[i].mode === mode)
                return i
        }

        return 0
    }

    function applyDisplayMode(mode) {
        graph.setDisplayMode(mode)
    }

    // En portrait, le sélecteur reste au-dessus du graphe.
    // En paysage plein écran, il est dessiné dans l'overlay pour maximiser
    // la surface utile du graphe.
    RowLayout {
        id: topModeSelector
        visible: root.showModeSelector && !root.overlayModeSelector
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Text {
            text: "Ordonnée :"
            color: root.textColorValue
        }

        ComboBox {
            id: topModeComboBox
            model: root.yModeOptions
            textRole: "label"
            currentIndex: root.modeIndexFor(root.displayMode)

            onActivated: function(index) {
                root.applyDisplayMode(root.yModeOptions[index].mode)
            }
        }
    }

    Item {
        id: graphContainer
        anchors.top: topModeSelector.visible ? topModeSelector.bottom : parent.top
        anchors.topMargin: topModeSelector.visible ? 8 : 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        GraphsView {
            id: graph
            anchors.fill: parent

            property int currentDisplayMode: root.displayMode
            property bool gridUpdatePending: false
            property var yVisibleTicks: []

            readonly property real targetMajorTickWidth: 100
            readonly property real targetMajorTickHeight: 80

            readonly property real yTickToGridGap: 25
            readonly property real yTickReservedWidth: 20
            readonly property real yLegendToTickGap: 10
            readonly property real yLegendReservedWidth: 10

            readonly property real baseXMin: 0
            readonly property real baseXMax: Math.max(1, root.dive.diveTime)
            readonly property real baseYMin: myDataSeries.displayMin
            readonly property real baseYMax: myDataSeries.displayMax

            function setDisplayMode(mode) {
                currentDisplayMode = mode
                root.displayMode = mode
                myDataSeries.setDisplayMode(mode)
                setClampedView(baseXMin, baseXMax, baseYMin, baseYMax)
                scheduleGridUpdate()
            }

            function yAxisLegendText() {
                switch (currentDisplayMode) {
                case root.displayTemperature:
                    return "Temperature (°C)"
                case root.displaySpeed:
                    return "Vitesse (m/min)"
                case root.displayDepth:
                default:
                    return "Profondeur (m)"
                }
            }

            function yAxisLabelDecimals() {
                switch (currentDisplayMode) {
                case root.displayTemperature:
                case root.displaySpeed:
                    return 1
                case root.displayDepth:
                default:
                    return 0
                }
            }

            function formatYAxisValue(value) {
                switch (currentDisplayMode) {
                case root.displayTemperature:
                case root.displaySpeed:
                    return Number(value).toFixed(axisY.labelDecimals)
                case root.displayDepth:
                default:
                    return Math.abs(value).toFixed(axisY.labelDecimals)
                }
            }

            function pickTimeStep(rawStep) {
                const safeRawStep = Math.max(1, rawStep)
                const steps = [5, 10, 15, 30, 60, 120, 300, 600, 900, 1800, 3600, 7200]

                for (let i = 0; i < steps.length; ++i) {
                    if (safeRawStep <= steps[i])
                        return steps[i]
                }

                return Math.ceil(safeRawStep / 3600) * 3600
            }

            function pickMetricStep(rawStep) {
                const safeRawStep = Math.max(1, rawStep)
                const power = Math.pow(10, Math.floor(Math.log10(safeRawStep)))
                const normalized = safeRawStep / power

                if (normalized <= 1)
                    return power
                if (normalized <= 2)
                    return 2 * power
                if (normalized <= 5)
                    return 5 * power

                return 10 * power
            }

            function pickTimeSubTickCount(step) {
                switch (step) {
                case 10:
                    return 1
                case 15:
                case 30:
                    return 2
                case 60:
                case 120:
                    return 3
                case 300:
                case 600:
                    return 4
                case 900:
                case 1800:
                    return 2
                case 3600:
                    return 5
                default:
                    return 0
                }
            }

            function pickMetricSubTickCount(step) {
                switch (step) {
                case 2:
                    return 1
                case 5:
                case 10:
                case 50:
                case 100:
                    return 4
                case 20:
                    return 3
                default:
                    return 0
                }
            }

            function buildVisibleTicks(minValue, maxValue, tickAnchor, tickInterval) {
                if (tickInterval <= 0 || maxValue <= minValue)
                    return []

                const epsilon = tickInterval * 0.000001
                const firstTick = tickAnchor + Math.ceil((minValue - tickAnchor - epsilon) / tickInterval) * tickInterval
                const ticks = []

                for (let value = firstTick; value <= maxValue + epsilon; value += tickInterval)
                    ticks.push(Number(value.toFixed(6)))

                return ticks
            }

            function clampAxisRange(currentMin, currentMax, boundMin, boundMax) {
                const maxRange = boundMax - boundMin
                let minValue = currentMin
                let maxValue = currentMax
                const currentRange = maxValue - minValue

                if (currentRange >= maxRange)
                    return { min: boundMin, max: boundMax }

                if (minValue < boundMin) {
                    const shift = boundMin - minValue
                    minValue += shift
                    maxValue += shift
                }

                if (maxValue > boundMax) {
                    const shift = maxValue - boundMax
                    minValue -= shift
                    maxValue -= shift
                }

                return { min: minValue, max: maxValue }
            }

            function setClampedView(xMin, xMax, yMin, yMax) {
                const clampedX = clampAxisRange(xMin, xMax, baseXMin, baseXMax)
                const clampedY = clampAxisRange(yMin, yMax, baseYMin, baseYMax)

                axisX.min = clampedX.min
                axisX.max = clampedX.max
                axisY.min = clampedY.min
                axisY.max = clampedY.max
            }

            function scheduleGridUpdate() {
                if (gridUpdatePending)
                    return

                gridUpdatePending = true
                Qt.callLater(function() {
                    gridUpdatePending = false
                    updateGrid()
                })
            }

            function updateGrid() {
                if (graph.width <= 0 || graph.height <= 0)
                    return

                const xRange = Math.max(1, axisX.max - axisX.min)
                const yRange = Math.max(1, axisY.max - axisY.min)
                const targetXTickCount = Math.max(2, graph.width / targetMajorTickWidth)
                const targetYTickCount = Math.max(2, graph.height / targetMajorTickHeight)
                const xStep = pickTimeStep(xRange / targetXTickCount)
                const yStep = pickMetricStep(yRange / targetYTickCount)

                axisX.tickAnchor = 0
                axisY.tickAnchor = 0
                axisX.tickInterval = xStep
                axisY.tickInterval = yStep
                axisX.subTickCount = pickTimeSubTickCount(xStep)
                axisY.subTickCount = pickMetricSubTickCount(yStep)
                yVisibleTicks = buildVisibleTicks(axisY.min, axisY.max, axisY.tickAnchor, axisY.tickInterval)
            }

            onWidthChanged: scheduleGridUpdate()
            onHeightChanged: scheduleGridUpdate()

            theme: GraphsTheme {
                readonly property color c1: "#AAAAAA"
                readonly property color c2: "#333333"
                readonly property color c3: Qt.lighter(c2, 2)

                colorScheme: Qt.styleHints.colorScheme === Qt.Dark
                    ? GraphsTheme.ColorScheme.Dark
                    : GraphsTheme.ColorScheme.Light
                grid.mainColor: c3
                grid.subColor: c2
                axisX.mainColor: c3
                axisY.mainColor: c3
                axisX.subColor: c2
                axisY.subColor: c2
                axisX.labelTextColor: c1
                axisY.labelTextColor: c1
            }

            axisX: ValueAxis {
                id: axisX
                min: graph.baseXMin
                max: graph.baseXMax
                tickAnchor: 0
                tickInterval: 60
                subTickCount: 3
                labelDecimals: 0
                onRangeChanged: graph.scheduleGridUpdate()
            }

            axisY: ValueAxis {
                id: axisY
                min: graph.baseYMin
                max: graph.baseYMax
                tickAnchor: 0
                tickInterval: 5
                subTickCount: 4
                labelDecimals: graph.yAxisLabelDecimals()
                labelsVisible: false
                onRangeChanged: graph.scheduleGridUpdate()
            }

            DragHandler {
                id: dragHandler
                target: null

                property point lastPoint: Qt.point(0, 0)

                onActiveChanged: {
                    if (active)
                        lastPoint = centroid.position
                }

                onCentroidChanged: {
                    if (!active)
                        return

                    const dxPixels = centroid.position.x - lastPoint.x
                    const dyPixels = centroid.position.y - lastPoint.y
                    const dxAxis = dxPixels * (axisX.max - axisX.min) / parent.width
                    const dyAxis = dyPixels * (axisY.max - axisY.min) / parent.height

                    graph.setClampedView(
                        axisX.min - dxAxis,
                        axisX.max - dxAxis,
                        axisY.min + dyAxis,
                        axisY.max + dyAxis
                    )

                    lastPoint = centroid.position
                }
            }

            PinchHandler {
                id: pinchHandler
                target: null

                property real lastScale: 1.0
                property point lastCentroid: Qt.point(0, 0)

                onActiveChanged: {
                    if (active) {
                        lastScale = 1.0
                        lastCentroid = centroid.position
                    }
                }

                onActiveScaleChanged: {
                    const frameDelta = activeScale / lastScale
                    const adjustedDelta = 1.0 + (frameDelta - 1.0)
                    const currentXRange = axisX.max - axisX.min
                    const currentYRange = axisY.max - axisY.min
                    const newXRange = currentXRange / adjustedDelta
                    const newYRange = currentYRange / adjustedDelta
                    const xFactor = (centroid.position.x - pinchHandler.parent.x) / pinchHandler.parent.width
                    const yFactor = 1.0 - (centroid.position.y - pinchHandler.parent.y) / pinchHandler.parent.height

                    const nextXMin = axisX.min + (currentXRange - newXRange) * xFactor
                    const nextYMin = axisY.min + (currentYRange - newYRange) * yFactor

                    graph.setClampedView(
                        nextXMin,
                        nextXMin + newXRange,
                        nextYMin,
                        nextYMin + newYRange
                    )

                    lastScale = activeScale
                }

                onCentroidChanged: {
                    if (!active)
                        return

                    const dx = (centroid.position.x - lastCentroid.x) * (axisX.max - axisX.min) / pinchHandler.parent.width
                    const dy = (centroid.position.y - lastCentroid.y) * (axisY.max - axisY.min) / pinchHandler.parent.height

                    graph.setClampedView(
                        axisX.min - dx,
                        axisX.max - dx,
                        axisY.min + dy,
                        axisY.max + dy
                    )

                    lastCentroid = centroid.position
                }
            }

            SampleModel {
                id: myDataSeries
                color: root.accentColorValue
                width: 4
            }

            Component.onCompleted: {
                graph.setDisplayMode(root.displayMode)
                myDataSeries.setEntries(root.dive.id)
                graph.setClampedView(graph.baseXMin, graph.baseXMax, graph.baseYMin, graph.baseYMax)
                graph.scheduleGridUpdate()
            }
        }

        Item {
            id: graphOverlay
            anchors.fill: parent

            function yTickX(labelWidth) {
                return Math.max(
                    yLegendContainer.x + yLegendContainer.width + graph.yLegendToTickGap,
                    graph.plotArea.x - labelWidth - graph.yTickToGridGap
                )
            }

            function yTickY(value, labelHeight) {
                const axisRange = Math.max(1, axisY.max - axisY.min)
                const tickRatio = (axisY.max - value) / axisRange
                return graph.plotArea.y + graph.plotArea.height * tickRatio - labelHeight / 2
            }

            function yLegendX(containerWidth) {
                return Math.max(
                    0,
                    graph.plotArea.x
                    - graph.yTickReservedWidth
                    - graph.yTickToGridGap
                    - graph.yLegendToTickGap
                    - containerWidth
                )
            }

            Text {
                id: xAxisLegend
                x: graph.plotArea.x + (graph.plotArea.width - width) / 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4
                text: "Temps (s)"
                color: "#AAAAAA"
            }

            Repeater {
                model: graph.yVisibleTicks

                Text {
                    required property real modelData

                    x: graphOverlay.yTickX(width)
                    y: graphOverlay.yTickY(modelData, height)
                    text: graph.formatYAxisValue(modelData)
                    color: "#AAAAAA"
                }
            }

            Item {
                id: yLegendContainer
                width: graph.yLegendReservedWidth
                height: graph.plotArea.height
                x: graphOverlay.yLegendX(width)
                y: graph.plotArea.y

                Text {
                    anchors.centerIn: parent
                    rotation: -90
                    text: graph.yAxisLegendText()
                    color: "#AAAAAA"
                }
            }

            Button {
                id: expandButton
                visible: root.showExpandButton
                x: graph.plotArea.x + graph.plotArea.width - width - 8
                y: graph.plotArea.y + 8
                text: "Agrandir"

                onClicked: root.expandRequested()
            }

            RowLayout {
                id: overlayModeSelector
                visible: root.showModeSelector && root.overlayModeSelector
                x: graph.plotArea.x + (graph.plotArea.width - width) / 2
                y: graph.plotArea.y + 8
                spacing: 10

                Text {
                    text: "Ordonnée :"
                    color: root.textColorValue
                }

                ComboBox {
                    id: overlayModeComboBox
                    model: root.yModeOptions
                    textRole: "label"
                    currentIndex: root.modeIndexFor(root.displayMode)

                    onActivated: function(index) {
                        root.applyDisplayMode(root.yModeOptions[index].mode)
                    }
                }
            }
        }
    }
}
