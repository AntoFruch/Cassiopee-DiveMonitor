import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    required property var deviceState

    property string title: "Settings"
    property bool hideSettingsButton: true

    Rectangle {
        anchors.fill: parent
        color: appColors.bgColor
    }

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: contentColumn.implicitHeight + 24
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: contentColumn
            width: parent.width
            spacing: 18

            Item {
                width: parent.width
                height: 20
            }

            Column {
                width: parent.width - 32
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 6

                Label {
                    width: parent.width
                    text: "App and dive computer configuration"
                    color: Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.65)
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                }
            }

            Column {
                width: parent.width
                spacing: 8

                Label {
                    x: 16
                    text: "Device"
                    color: appColors.accentColor
                    font.pixelSize: 18
                    font.bold: true
                }

                Rectangle {
                    width: parent.width
                    height: deviceRow.height
                    color: appColors.bgColor

                    Column {
                        anchors.fill: parent
                        spacing: 0

                        ItemDelegate {
                            id: deviceRow
                            width: parent.width
                            height: 64
                            leftPadding: 16
                            rightPadding: 16
                            onClicked: devicePickerDialog.openDialog()

                            background: Rectangle {
                                color: "transparent"
                            }

                            contentItem: RowLayout {
                                spacing: 12

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4

                                    Label {
                                        Layout.fillWidth: true
                                        text: "Dive computer"
                                        color: appColors.textColor
                                        font.pixelSize: 17
                                        font.bold: true
                                        elide: Text.ElideRight
                                    }

                                    Label {
                                        Layout.fillWidth: true
                                        text: deviceState.selectedModel
                                              ? deviceState.selectedBrand + " - " + deviceState.selectedModel
                                              : "Choisir un appareil"
                                        color: Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.65)
                                        font.pixelSize: 14
                                        elide: Text.ElideRight
                                    }
                                }

                                Label {
                                    text: ">"
                                    color: Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.4)
                                    font.pixelSize: 18
                                    Layout.alignment: Qt.AlignVCenter
                                }
                            }
                        }

                        Rectangle {
                            width: parent.width - 16
                            height: 1
                            x: 16
                            color: appColors.separatorColor
                        }
                    }
                }
            }

            Column {
                width: parent.width
                spacing: 8

                Label {
                    x: 16
                    text: "Appearance"
                    color: appColors.accentColor
                    font.pixelSize: 18
                    font.bold: true
                }

                Rectangle {
                    width: parent.width
                    height: 82
                    color: appColors.bgColor

                    Column {
                        anchors.fill: parent
                        spacing: 0

                        Item {
                            width: parent.width
                            height: 64

                            Label {
                                text: "Theme"
                                anchors.left: parent.left
                                anchors.leftMargin: 16
                                anchors.verticalCenter: parent.verticalCenter
                                color: appColors.textColor
                                font.pixelSize: 17
                                font.bold: true
                            }

                            ComboBox {
                                id: themeComboBox
                                anchors.right: parent.right
                                anchors.rightMargin: 16
                                anchors.verticalCenter: parent.verticalCenter
                                width: 140
                                model: ["System", "Light", "Dark"]
                                currentIndex: userPrefs.theme === Qt.Light ? 1 : (userPrefs.theme === Qt.Dark ? 2 : 0)

                                onActivated: function(index) {
                                    if (index === 0) {
                                        userPrefs.theme = Qt.Unknown
                                    } else if (index === 1) {
                                        userPrefs.theme = Qt.Light
                                    } else {
                                        userPrefs.theme = Qt.Dark
                                    }
                                }
                            }
                        }

                        Rectangle {
                            width: parent.width - 16
                            height: 1
                            x: 16
                            color: appColors.separatorColor
                        }
                    }
                }
            }
        }
    }

    DevicePickerDialog {
        id: devicePickerDialog
        deviceState: root.deviceState
    }
}
