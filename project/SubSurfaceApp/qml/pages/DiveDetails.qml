import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts

Item {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    required property var dive
    property string title: dive.date

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // --- Debug Header ---
        Label {
            text: "Samples for Dive ID: " + dive.id
            font.bold: true
            font.pixelSize: 18
        }

        // --- Samples List ---
        ListView {
            id: samplesList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            // Connect to the model in your dcWrapper
            model: dcWrapper.samplesModel

            // Header row for the list
            header: RowLayout {
                width: parent.width
                height: 30
                Label { text: "Time (s)"; Layout.fillWidth: true; font.bold: true }
                Label { text: "Depth (m)"; Layout.fillWidth: true; font.bold: true }
                Label { text: "Speed (m/s)"; Layout.fillWidth: true; font.bold: true }
                Label { text: "Temp (°C)"; Layout.fillWidth: true; font.bold: true }
            }

            // Visual row for every sample point
            delegate: ItemDelegate {
                width: samplesList.width

                contentItem: RowLayout {
                    Label {
                        text: model.time
                        Layout.fillWidth: true
                    }
                    Label {
                        text: model.depth.toFixed(2) + " m"
                        Layout.fillWidth: true
                    }
                    Label {
                        text: model.velocity.toFixed(2) + " m/s"
                        Layout.fillWidth: true
                    }
                    Label {
                        text: model.temp ? model.temp + "°C" : "--"
                        Layout.fillWidth: true
                    }
                }
            }

            // Simple scrollbar
            ScrollBar.vertical: ScrollBar {
                active: true
            }

            // Empty state message
            Label {
                anchors.centerIn: parent
                text: "No samples found for this dive."
                visible: samplesList.count === 0
                color: "gray"
            }
        }
    }
}