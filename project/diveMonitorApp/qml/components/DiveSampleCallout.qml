import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    required property real timeSeconds
    required property real depthMeters
    required property real temperatureCelsius
    required property bool hasTemperature
    required property real speedMetersPerMinute

    radius: 12
    color: appColors.bgColor
    border.width: 2
    border.color: appColors.accentColor
    implicitWidth: contentColumn.implicitWidth + 24
    implicitHeight: contentColumn.implicitHeight + 20

    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: 10
        spacing: 4

        Text {
            text: "Temps: " + Math.round(root.timeSeconds) + " s"
            color: appColors.textColor
            font.bold: true
        }

        Text {
            text: "Profondeur: " + Number(root.depthMeters).toFixed(1) + " m"
            color: appColors.textColor
        }

        Text {
            text: "Temperature: "
                  + (root.hasTemperature
                     ? Number(root.temperatureCelsius).toFixed(1) + " \u00B0C"
                     : "--")
            color: appColors.textColor
        }

        Text {
            text: "Vitesse: " + Number(root.speedMetersPerMinute).toFixed(1) + " m/min"
            color: appColors.textColor
        }
    }
}
