import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root

    required property var deviceState

    property string pendingDeviceBrand: ""
    property string pendingDeviceModel: ""
    property string pendingConnectionMode: ""

    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.max(60, Math.round((parent.height - height) / 2))
    width: Math.min(parent.width - 24, 320)
    modal: true
    focus: true
    padding: 0

    function openDialog() {
        pendingDeviceBrand = deviceState.selectedBrand
        pendingDeviceModel = deviceState.selectedModel
        pendingConnectionMode = deviceState.selectedConnectionMode
        open()
    }

    function applySelection() {
        deviceState.selectedBrand = pendingDeviceBrand
        deviceState.selectedModel = pendingDeviceModel
        deviceState.selectedConnectionMode = pendingConnectionMode
        close()
    }

    background: Rectangle {
        radius: 18
        color: bgColor
        border.width: 2
        border.color: accentColor
    }

    contentItem: Rectangle {
        radius: 18
        color: "transparent"
        clip: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 16

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 58
                color: "transparent"

                Label {
                    anchors.centerIn: parent
                    text: "Choose device"
                    font.pixelSize: 18
                    font.bold: true
                    color: textColor
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 18
                Layout.rightMargin: 18
                spacing: 14

                Label {
                    text: "Brand"
                    font.bold: true
                    color: textColor
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: brandComboBox
                    model: root.deviceState.deviceBrands
                    currentIndex: Math.max(0, root.deviceState.deviceBrands.indexOf(root.pendingDeviceBrand))
                    Layout.fillWidth: true

                    onActivated: function(index) {
                        root.pendingDeviceBrand = root.deviceState.deviceBrands[index]
                        root.pendingDeviceModel = root.deviceState.modelsForBrand(root.pendingDeviceBrand)[0] || ""
                    }
                }

                Label {
                    text: "Device"
                    font.bold: true
                    color: textColor
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: deviceModelComboBox
                    model: root.deviceState.modelsForBrand(root.pendingDeviceBrand)
                    currentIndex: Math.max(0, model.indexOf(root.pendingDeviceModel))
                    Layout.fillWidth: true

                    onActivated: function(index) {
                        root.pendingDeviceModel = model[index]
                    }
                }

                Label {
                    text: "Connection mode"
                    font.bold: true
                    color: textColor
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: connectionComboBox
                    model: root.deviceState.connectionModes
                    currentIndex: Math.max(0, root.deviceState.connectionModes.indexOf(root.pendingConnectionMode))
                    Layout.fillWidth: true

                    onActivated: function(index) {
                        root.pendingConnectionMode = root.deviceState.connectionModes[index]
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 68
                color: "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 18
                    anchors.rightMargin: 18
                    anchors.bottomMargin: 16
                    spacing: 12

                    Button {
                        text: "Cancel"
                        Layout.fillWidth: true
                        onClicked: root.close()
                    }

                    Button {
                        text: "OK"
                        Layout.fillWidth: true

                        background: Rectangle {
                            radius: 10
                            color: accentColor
                        }

                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: root.applySelection()
                    }
                }
            }
        }
    }
}
