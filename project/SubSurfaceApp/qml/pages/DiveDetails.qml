import QtQuick
import QtGraphs
import QtQuick.Layouts
import DiveMonitorCustom 1.0

ColumnLayout{
    id: root
    height: parent.height
    width: parent.width

    // on recupere la plongée selectionnée dans la liste (les points sont pas dedans sinon c'est trop lourd)
    required property var dive
    property string title: dive.date

    // ca definit un graphe tout fait
    GraphsView{
        id: graph

        Layout.fillWidth: true          // prend toute la largeur
        Layout.alignment: Qt.AlignTop   //alignement haut

        height: parent.height * 0.5     // moitié de la hauteur du parent
        theme: GraphsTheme {
            // Ici c'est que pour jouer sur l'aspect visuel du graphe,couleur etc, checker les propriete de GraphsTheme pour voir ce qu'on peut faire.
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

        // AXES | on peut faire varier les intervales, à voir pour que ca soit fait en fontion de la pongée pour que ca soit bien
        // axe X
        axisX: ValueAxis {
            id: axisX
            max: 600
            tickInterval: 60
            subTickCount: 5
            labelDecimals: 0

        }
        // Axe Y
        axisY: ValueAxis {
            id: axisY
            max: dive.maxDepth
            tickInterval: 5
            subTickCount: 4
            labelDecimals: 1
        }

        // pour se deplacer dans le graphe ( Généré par Gemini)
        DragHandler {
            id: dragHandler
            target: null

            // On stocke la dernière position pour calculer un delta fluide
            property point lastPoint: Qt.point(0, 0)

            onActiveChanged: {
                if (active) {
                    lastPoint = centroid.position
                }
            }

            onCentroidChanged: {
                if (active) {
                    // 1. Calcul du déplacement en pixels depuis la dernière frame
                    let dxPixels = centroid.position.x - lastPoint.x
                    let dyPixels = centroid.position.y - lastPoint.y

                    // 2. Conversion des pixels en unités de l'axe
                    // On multiplie par la plage actuelle (max - min) / taille du composant
                    let dxAxis = dxPixels * (axisX.max - axisX.min) / parent.width
                    let dyAxis = dyPixels * (axisY.max - axisY.min) / parent.height

                    // 3. Application du mouvement (Inversion de dy car Y écran descend)
                    axisX.min -= dxAxis
                    axisX.max -= dxAxis
                    axisY.min += dyAxis
                    axisY.max += dyAxis

                    // 4. Mise à jour du point de référence
                    lastPoint = centroid.position
                }
            }
        }

        // pour zoomer/dezoomer dans le graphe ( Généré par Gemini)
        PinchHandler {
            id: pinchHandler
            target: null

            property real lastScale: 1.0
            // On stocke la position du centre du pincement pour calculer le déplacement
            property point lastCentroid: Qt.point(0, 0)

            onActiveChanged: {
                if (active) {
                    lastScale = 1.0
                    lastCentroid = centroid.position
                }
            }

            onActiveScaleChanged: {
                // 1. Calcul du delta de zoom avec sensibilité
                let frameDelta = activeScale / lastScale
                let sensitivity = 1
                let adjustedDelta = 1.0 + (frameDelta - 1.0) * sensitivity

                // 2. Calcul des nouvelles plages (ranges)
                let currentXRange = axisX.max - axisX.min
                let currentYRange = axisY.max - axisY.min
                let newXRange = currentXRange / adjustedDelta
                let newYRange = currentYRange / adjustedDelta

                // 3. Zoom focalisé sur le centroïde
                // On calcule où se trouve le doigt par rapport aux bornes actuelles (0.0 à 1.0)
                let xFactor = (centroid.position.x - pinchHandler.parent.x) / pinchHandler.parent.width
                let yFactor = 1.0 - (centroid.position.y - pinchHandler.parent.y) / pinchHandler.parent.height
                // Note: yFactor est inversé si votre axe Y croît vers le haut (standard en ChartView)

                // On repositionne les bornes pour que le point sous le doigt reste immobile
                axisX.min = axisX.min + (currentXRange - newXRange) * xFactor
                axisX.max = axisX.min + newXRange

                axisY.min = axisY.min + (currentYRange - newYRange) * yFactor
                axisY.max = axisY.min + newYRange

                lastScale = activeScale
            }

            onCentroidChanged: {
                if (active) {
                    // Gère le déplacement (pan) pendant le pincement
                    let dx = (centroid.position.x - lastCentroid.x) * (axisX.max - axisX.min) / pinchHandler.parent.width
                    let dy = (centroid.position.y - lastCentroid.y) * (axisY.max - axisY.min) / pinchHandler.parent.height

                    axisX.min -= dx
                    axisX.max -= dx
                    axisY.min += dy // + car l'axe Y des coordonnées écran est inversé par rapport au graphique
                    axisY.max += dy

                    lastCentroid = centroid.position
                }
            }
        }

        // composant custom (voir la classe C++ SampleModel)
        DiveSeries {
                id: myDataSeries
                color: accentColor
                width: 4
            }

            Component.onCompleted: {
                myDataSeries.setEntries(dive.id);
            }
    }
}