import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts

Item {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    required property var deviceState
    property string title: "Add Dives"

    Rectangle {
        anchors.fill: parent
        color: bgColor
    }

    Rectangle {
        width: Math.min(parent.width - 28, 400)
        height: 320
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 110
        radius: 24
        color: bgColor
        border.width: 3
        border.color: accentColor

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 22
            spacing: 18

            Label {
                text: "Import Dives"
                font.pixelSize: 22
                font.bold: true
                color: accentColor
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Label {
                text: "your current device :"
                font.pixelSize: 14
                font.bold: true
                color: textColor
                Layout.fillWidth: true
            }

            Label {
                text: deviceState.selectedBrand  ? deviceState.selectedBrand + " " + deviceState.selectedModel : "No Device chosen, choose one";
                font.pixelSize: 22
                font.bold: true
                color: textColor
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            RowLayout {
                spacing: 8
                Layout.fillWidth: true

                Rectangle {
                    width: 16
                    height: 16
                    radius: 8
                    color: deviceState.isDeviceConnected ? "#7ED957" : "#E05050"
                }

                Label {
                    text: deviceState.isDeviceConnected ? "Device connected" : "Device not connected"
                    font.pixelSize: 14
                    color: textColor
                    Layout.fillWidth: true
                }
            }

            Button {
                text: deviceState.isDeviceConnected ? "Disconnect" : "Connect Device"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 200
                Layout.preferredHeight: 45

                // Only allow clicking if a model is actually selected
                enabled: deviceState.selectedModel !== ""

                background: Rectangle {
                    radius: 12
                    // Blue-ish if connecting, Red-ish if disconnecting
                    color: !deviceState.isDeviceConnected ? "#3498db" : "#e74c3c"
                    opacity: parent.enabled ? 1.0 : 0.5
                }

                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 14
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    if (!deviceState.isDeviceConnected) {
                        dcWrapper.connectToDevice(deviceState.selectedBrand, deviceState.selectedModel)
                    } else {
                        dcWrapper.disconnectDevice()
                    }
                }
            }

            // ... existing Spacing / Import Dives Button ...

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 4
            }

            Button {
                text: "change device"
                Layout.alignment: Qt.AlignHCenter

                background: Rectangle {
                    radius: 14
                    color: "transparent"
                    border.width: 2
                    border.color: accentColor
                }

                contentItem: Text {
                    text: parent.text
                    color: accentColor
                    font.pixelSize: 14
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: devicePickerDialog.openDialog()
            }

            Item {
                Layout.fillHeight: true
            }

            Button {
                text: "Import Dives"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 206
                Layout.preferredHeight: 62

                enabled: dcWrapper.connected

                background: Rectangle {
                    radius: 16
                    color: parent.enabled ? accentColor : "#95a5a6"
                }

                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? "white" : "#ecf0f1"
                    font.pixelSize: 18
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: dcWrapper.importDives()
            }
        }
    }

    DevicePickerDialog {
        id: devicePickerDialog
        deviceState: root.deviceState
    }
}
