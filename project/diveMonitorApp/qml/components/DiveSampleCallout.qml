import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    required property real timeSeconds
    required property real depthMeters
    required property real temperatureCelsius
    required property bool hasTemperature
    required property real speedMetersPerMinute
    property bool rangeMode: false
    property real durationSeconds: 0
    property real deltaDepthMeters: 0
    property real averageSpeedMetersPerMinute: 0

    function signedFixed(value, decimals) {
        const numberValue = Number(value)
        const sign = numberValue > 0 ? "+" : ""
        return sign + numberValue.toFixed(decimals)
    }

    function formattedMinutesSeconds(seconds) {
        const totalSeconds = Math.max(0, Math.round(Number(seconds)))
        const minutes = Math.floor(totalSeconds / 60)
        const remainingSeconds = totalSeconds % 60

        return minutes + " min " + remainingSeconds + " s"
    }

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
            visible: !root.rangeMode
            text: "Temps: " + root.formattedMinutesSeconds(root.timeSeconds)
            color: appColors.textColor
            font.bold: true
        }

        Text {
            visible: !root.rangeMode
            text: "Profondeur: " + Number(root.depthMeters).toFixed(1) + " m"
            color: appColors.textColor
        }

        Text {
            visible: !root.rangeMode
            text: "Temperature: "
                  + (root.hasTemperature
                     ? Number(root.temperatureCelsius).toFixed(1) + " \u00B0C"
                     : "--")
            color: appColors.textColor
        }

        Text {
            visible: !root.rangeMode
            text: "Vitesse: " + Number(root.speedMetersPerMinute).toFixed(1) + " m/min"
            color: appColors.textColor
        }

        Text {
            visible: root.rangeMode
            text: "Duree: " + root.formattedMinutesSeconds(root.durationSeconds)
            color: appColors.textColor
            font.bold: true
        }

        Text {
            visible: root.rangeMode
            text: "Delta profondeur: " + root.signedFixed(root.deltaDepthMeters, 1) + " m"
            color: appColors.textColor
        }

        Text {
            visible: root.rangeMode
            text: "Vitesse moyenne: "
                  + root.signedFixed(root.averageSpeedMetersPerMinute, 1)
                  + " m/min"
            color: appColors.textColor
        }
    }
}
