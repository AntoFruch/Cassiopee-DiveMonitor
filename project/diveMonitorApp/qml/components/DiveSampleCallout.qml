import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    required property color accentColorValue
    required property color backgroundColorValue
    required property color textColorValue
    required property real timeSeconds
    required property real depthMeters
    required property real temperatureCelsius
    required property bool hasTemperature
    required property real speedMetersPerMinute

    radius: 12
    color: root.backgroundColorValue
    border.width: 2
    border.color: root.accentColorValue
    implicitWidth: contentColumn.implicitWidth + 24
    implicitHeight: contentColumn.implicitHeight + 20

    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: 10
        spacing: 4

        Text {
            text: "Temps: " + Math.round(root.timeSeconds) + " s"
            color: root.textColorValue
            font.bold: true
        }

        Text {
            text: "Profondeur: " + Number(root.depthMeters).toFixed(1) + " m"
            color: root.textColorValue
        }

        Text {
            text: "Temperature: "
                  + (root.hasTemperature
                     ? Number(root.temperatureCelsius).toFixed(1) + " \u00B0C"
                     : "--")
            color: root.textColorValue
        }

        Text {
            text: "Vitesse: " + Number(root.speedMetersPerMinute).toFixed(1) + " m/min"
            color: root.textColorValue
        }
    }
}
