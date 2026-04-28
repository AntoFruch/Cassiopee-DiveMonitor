import QtQuick
import QtGraphs
import QtQuick.Layouts
import DiveMonitorCustom 1.0

ColumnLayout {
    id: root
    height: parent.height
    width: parent.width

    // On récupère la plongée sélectionnée dans la liste.
    // Les points ne sont pas embarqués dans cet objet pour éviter de l'alourdir.
    required property var dive
    property string title: dive.date

    // Cette zone contient le graphe QtGraphs et les éléments QML dessinés autour
    // (légendes et labels Y personnalisés).
    Item {
        id: graphContainer

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        height: parent.height * 0.5

        // GraphsView gère le rendu du graphe, des axes, de la grille et des gestes.
        GraphsView {
            id: graph
            anchors.fill: parent

            // Ces constantes doivent rester alignées avec l'enum C++ DisplayMode
            // utilisé dans SampleModel.
            readonly property int displayDepth: 0
            readonly property int displaySpeed: 1
            readonly property int displayTemperature: 2

            property int currentDisplayMode: displayDepth
            // Empêche plusieurs recalculs redondants de la grille dans la même
            // boucle d'événements.
            property bool gridUpdatePending: false
            // Liste des graduations Y visibles. On la calcule nous-mêmes pour
            // pouvoir dessiner les labels Y à la bonne position.
            property var yVisibleTicks: []

            // Taille cible des grandes cases de grille à l'écran.
            // Le tickInterval s'adapte ensuite pour garder une grille lisible.
            readonly property real targetMajorTickWidth: 100
            readonly property real targetMajorTickHeight: 80

            // Réglages de mise en page à gauche du graphe :
            // espace entre le quadrillage et les chiffres,
            // largeur réservée aux chiffres,
            // espace entre chiffres et légende,
            // largeur réservée à la légende.
            readonly property real yTickToGridGap: 25
            readonly property real yTickReservedWidth: 20
            readonly property real yLegendToTickGap: 10
            readonly property real yLegendReservedWidth: 10

            // Synchronise le mode affiché côté QML et côté série C++.
            function setDisplayMode(mode) {
                currentDisplayMode = mode
                myDataSeries.setDisplayMode(mode)
            }

            // Texte de la légende verticale de l'axe Y selon la grandeur affichée.
            function yAxisLegendText() {
                switch (currentDisplayMode) {
                case displayTemperature:
                    return "Temperature (°C)"
                case displaySpeed:
                    return "Vitesse"
                case displayDepth:
                default:
                    return "Profondeur (m)"
                }
            }

            // Les profondeurs sont tracées en négatif pour faire "descendre" la
            // courbe. Ici, on reformate la valeur pour l'afficher positivement.
            function formatYAxisValue(value) {
                return Math.abs(value).toFixed(axisY.labelDecimals)
            }

            // Choisit un pas de temps "propre" pour la grille :
            // 5 s, 10 s, 30 s, 1 min, 2 min, 5 min, etc.
            function pickTimeStep(rawStep) {
                const safeRawStep = Math.max(1, rawStep)
                const steps = [5, 10, 15, 30, 60, 120, 300, 600, 900, 1800, 3600, 7200]

                for (let i = 0; i < steps.length; ++i) {
                    if (safeRawStep <= steps[i])
                        return steps[i]
                }

                return Math.ceil(safeRawStep / 3600) * 3600
            }

            // Choisit un pas métrique "propre" suivant la règle 1 / 2 / 5 * 10^n.
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

            // Nombre de sous-graduations entre deux traits principaux en X.
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

            // Nombre de sous-graduations entre deux traits principaux en Y.
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

            // Construit la liste des graduations Y visibles dans la fenêtre
            // actuelle. Ces valeurs alimentent ensuite le Repeater de labels Y.
            function buildVisibleTicks(minValue, maxValue, tickAnchor, tickInterval) {
                if (tickInterval <= 0 || maxValue <= minValue)
                    return []

                // Petite marge pour éviter les erreurs d'arrondi flottant au bord
                // des graduations.
                const epsilon = tickInterval * 0.000001
                const firstTick = tickAnchor + Math.ceil((minValue - tickAnchor - epsilon) / tickInterval) * tickInterval
                const ticks = []

                for (let value = firstTick; value <= maxValue + epsilon; value += tickInterval)
                    ticks.push(Number(value.toFixed(6)))

                return ticks
            }

            // Demande un recalcul différé de la grille pour éviter de recalculer
            // plusieurs fois de suite pendant un drag ou un pinch.
            function scheduleGridUpdate() {
                if (gridUpdatePending)
                    return

                gridUpdatePending = true
                Qt.callLater(function() {
                    gridUpdatePending = false
                    updateGrid()
                })
            }

            // Recalcule les axes et la grille à partir de la fenêtre visible.
            // Plus on zoome, plus le tickInterval devient fin.
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

            // Si la taille du composant change, on réadapte la grille.
            onWidthChanged: scheduleGridUpdate()
            onHeightChanged: scheduleGridUpdate()

            theme: GraphsTheme {
                // Ici c'est uniquement l'aspect visuel du graphe.
                readonly property color c1: "#AAAAAA"
                readonly property color c2: "#333333"
                readonly property color c3: Qt.lighter(c2, 2)

                colorScheme: bgColor.toString()
                grid.mainColor: c3
                grid.subColor: c2
                axisX.mainColor: c3
                axisY.mainColor: c3
                axisX.subColor: c2
                axisY.subColor: c2
                axisX.labelTextColor: c1
                axisY.labelTextColor: c1
            }

            // Axe horizontal du temps.
            axisX: ValueAxis {
                id: axisX
                min: 0
                max: Math.max(1, dive.diveTime)
                tickAnchor: 0
                tickInterval: 60
                subTickCount: 3
                labelDecimals: 0
                onRangeChanged: graph.scheduleGridUpdate()
            }

            // Axe vertical.
            // La profondeur est représentée avec des valeurs négatives pour que
            // la courbe descende visuellement depuis la surface.
            axisY: ValueAxis {
                id: axisY
                // La série profondeur est tracée en négatif pour que la courbe descende.
                min: -Math.max(1, dive.maxDepth)
                max: 0
                tickAnchor: 0
                tickInterval: 5
                subTickCount: 4
                labelDecimals: 0
                // Les labels natifs sont masqués car on veut un contrôle plus fin
                // de leur position et de leur format.
                labelsVisible: false
                onRangeChanged: graph.scheduleGridUpdate()
            }

            // Déplacement du graphe à un doigt / à la souris.
            // On convertit un delta en pixels vers un delta dans l'espace des axes.
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

                    axisX.min -= dxAxis
                    axisX.max -= dxAxis
                    axisY.min += dyAxis
                    axisY.max += dyAxis

                    lastPoint = centroid.position
                }
            }

            // Zoom et déplacement à deux doigts.
            // Le zoom est centré autour du point de pinch pour garder une sensation naturelle.
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

                    axisX.min = axisX.min + (currentXRange - newXRange) * xFactor
                    axisX.max = axisX.min + newXRange
                    axisY.min = axisY.min + (currentYRange - newYRange) * yFactor
                    axisY.max = axisY.min + newYRange

                    lastScale = activeScale
                }

                onCentroidChanged: {
                    if (!active)
                        return

                    const dx = (centroid.position.x - lastCentroid.x) * (axisX.max - axisX.min) / pinchHandler.parent.width
                    const dy = (centroid.position.y - lastCentroid.y) * (axisY.max - axisY.min) / pinchHandler.parent.height

                    axisX.min -= dx
                    axisX.max -= dx
                    axisY.min += dy
                    axisY.max += dy

                    lastCentroid = centroid.position
                }
            }

            // Série C++ qui alimente le graphe en points.
            SampleModel {
                id: myDataSeries
                color: accentColor
                width: 4
            }

            // Initialisation du graphe :
            // 1. choisir le mode affiché
            // 2. charger les points de la plongée
            // 3. calculer la première grille
            Component.onCompleted: {
                graph.setDisplayMode(graph.displayDepth)
                myDataSeries.setEntries(dive.id)
                graph.scheduleGridUpdate()
            }
        }

        // Cet overlay QML est dessiné au-dessus du GraphsView.
        // Il sert à afficher les éléments que QtGraphs ne place pas exactement
        // comme on le souhaite : labels Y et légende de l'axe Y.
        Item {
            id: graphOverlay
            anchors.fill: parent

            // Position horizontale d'un label Y.
            // On le place au plus près du graphe, mais toujours à droite de la légende.
            function yTickX(labelWidth) {
                return Math.max(
                    yLegendContainer.x + yLegendContainer.width + graph.yLegendToTickGap,
                    graph.plotArea.x - labelWidth - graph.yTickToGridGap
                )
            }

            // Position verticale d'un label Y à partir de sa valeur d'axe.
            // On convertit la valeur dans le repère visuel du plotArea.
            function yTickY(value, labelHeight) {
                const axisRange = Math.max(1, axisY.max - axisY.min)
                const tickRatio = (axisY.max - value) / axisRange
                return graph.plotArea.y + graph.plotArea.height * tickRatio - labelHeight / 2
            }

            // Position horizontale de la légende verticale Y.
            // Elle reste toujours à gauche de la colonne de chiffres.
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

            // Légende de l'axe X.
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4
                text: "Temps (s)"
                color: "#AAAAAA"
            }

            // Labels Y personnalisés.
            // On les redessine nous-mêmes pour choisir leur texte et leur position.
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

            // Conteneur de la légende Y.
            // La rotation du texte est isolée ici pour simplifier le positionnement.
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
        }
    }
}
