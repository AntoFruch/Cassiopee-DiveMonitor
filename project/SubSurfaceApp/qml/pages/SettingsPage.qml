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
        anchors.margins: 0
        contentWidth: width
        contentHeight: contentColumn.implicitHeight + 40
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: contentColumn
            width: parent.width
            spacing: 22

            Rectangle {
                width: parent.width
                height: 24
                color: "transparent"
            }

            Column {
                width: parent.width - 40
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8

                Label {
                    width: parent.width
                    text: "Preferences"
                    color: appColors.textColor
                    font.pixelSize: 28
                    font.bold: true
                }

                Label {
                    width: parent.width
                    text: "Personnalise l'application et configure ton appareil de plongée."
                    color: Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.68)
                    wrapMode: Text.WordWrap
                    font.pixelSize: 14
                }
            }

            Rectangle {
                width: parent.width - 32
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 22
                color: appColors.listButtonColor
                border.width: 1
                border.color: Qt.rgba(appColors.separatorColor.r, appColors.separatorColor.g, appColors.separatorColor.b, 0.7)
                implicitHeight: cardContent.implicitHeight + 28

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 6
                    radius: 22
                    color: appColors.accentColor
                }

                Column {
                    id: cardContent
                    anchors.fill: parent
                    anchors.margins: 14
                    anchors.leftMargin: 20
                    spacing: 0

                    Label {
                        width: parent.width
                        text: "General"
                        color: Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.62)
                        font.pixelSize: 12
                        font.bold: true
                        leftPadding: 4
                        bottomPadding: 10
                    }

                    ItemDelegate {
                        width: parent.width
                        height: 86
                        leftPadding: 4
                        rightPadding: 8
                        topPadding: 12
                        bottomPadding: 12
                        background: Rectangle {
                            color: "transparent"
                            radius: 16
                        }
                        onClicked: devicePickerDialog.openDialog()

                        contentItem: RowLayout {
                            spacing: 12

                            Rectangle {
                                Layout.preferredWidth: 42
                                Layout.preferredHeight: 42
                                radius: 14
                                color: Qt.rgba(appColors.accentColor.r, appColors.accentColor.g, appColors.accentColor.b, 0.14)

                                Label {
                                    anchors.centerIn: parent
                                    text: "DC"
                                    color: appColors.accentColor
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 3

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
                                    color: deviceState.selectedModel
                                           ? Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.76)
                                           : appColors.accentColor
                                    font.pixelSize: 14
                                    elide: Text.ElideRight
                                }
                            }

                            Label {
                                text: ">"
                                color: Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.45)
                                font.pixelSize: 20
                                Layout.alignment: Qt.AlignVCenter
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: Qt.rgba(appColors.separatorColor.r, appColors.separatorColor.g, appColors.separatorColor.b, 0.55)
                    }

                    Item {
                        width: parent.width
                        height: 94

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 4
                            anchors.rightMargin: 4
                            anchors.topMargin: 12
                            anchors.bottomMargin: 8
                            spacing: 10

                            Label {
                                Layout.fillWidth: true
                                text: "Theme"
                                color: appColors.textColor
                                font.pixelSize: 17
                                font.bold: true
                            }

                            ComboBox {
                                id: themeComboBox
                                Layout.fillWidth: true
                                implicitHeight: 44
                                model: ["System", "Light", "Dark"]
                                currentIndex: userPrefs.theme === Qt.Light ? 1 : (userPrefs.theme === Qt.Dark ? 2 : 0)

                                onActivated: function(index) {
                                    if (index === 0) {
                                        userPrefs.theme = Qt.Unknown
                                        Qt.styleHints.colorScheme = Qt.Unknown
                                    } else if (index === 1) {
                                        userPrefs.theme = Qt.Light
                                        Qt.styleHints.colorScheme = Qt.Light
                                    } else {
                                        userPrefs.theme = Qt.Dark
                                        Qt.styleHints.colorScheme = Qt.Dark
                                    }
                                }

                                contentItem: Text {
                                    leftPadding: 14
                                    rightPadding: 36
                                    text: themeComboBox.displayText
                                    font.pixelSize: 14
                                    color: appColors.textColor
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }

                                indicator: Canvas {
                                    id: themeIndicator
                                    x: themeComboBox.width - width - 14
                                    y: themeComboBox.topPadding + (themeComboBox.availableHeight - height) / 2
                                    width: 12
                                    height: 8
                                    contextType: "2d"

                                    Connections {
                                        target: themeComboBox
                                        function onPressedChanged() { themeIndicator.requestPaint() }
                                    }

                                    onPaint: {
                                        context.reset()
                                        context.moveTo(0, 0)
                                        context.lineTo(width, 0)
                                        context.lineTo(width / 2, height)
                                        context.closePath()
                                        context.fillStyle = appColors.accentColor
                                        context.fill()
                                    }
                                }

                                background: Rectangle {
                                    radius: 14
                                    color: appColors.bgColor
                                    border.width: 1
                                    border.color: themeComboBox.popup.visible
                                                  ? appColors.accentColor
                                                  : Qt.rgba(appColors.separatorColor.r, appColors.separatorColor.g, appColors.separatorColor.b, 0.85)
                                }

                                popup: Popup {
                                    y: themeComboBox.height + 6
                                    width: themeComboBox.width
                                    padding: 6

                                    contentItem: ListView {
                                        clip: true
                                        implicitHeight: contentHeight
                                        model: themeComboBox.popup.visible ? themeComboBox.delegateModel : null
                                        currentIndex: themeComboBox.highlightedIndex
                                    }

                                    background: Rectangle {
                                        radius: 16
                                        color: appColors.bgColor
                                        border.width: 1
                                        border.color: Qt.rgba(appColors.separatorColor.r, appColors.separatorColor.g, appColors.separatorColor.b, 0.9)
                                    }
                                }

                                delegate: ItemDelegate {
                                    id: themeDelegate
                                    width: themeComboBox.width - 12
                                    height: 42
                                    text: modelData
                                    highlighted: themeComboBox.highlightedIndex === index

                                    background: Rectangle {
                                        radius: 12
                                        color: themeDelegate.highlighted
                                               ? Qt.rgba(appColors.accentColor.r, appColors.accentColor.g, appColors.accentColor.b, 0.12)
                                               : "transparent"
                                    }

                                    contentItem: Text {
                                        text: modelData
                                        color: appColors.textColor
                                        font.pixelSize: 14
                                        verticalAlignment: Text.AlignVCenter
                                        leftPadding: 10
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width - 40
                anchors.horizontalCenter: parent.horizontalCenter
                color: "transparent"
                implicitHeight: helperContent.implicitHeight

                Column {
                    id: helperContent
                    width: parent.width
                    spacing: 6

                    Label {
                        text: "Tip"
                        color: appColors.accentColor
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Label {
                        width: parent.width
                        text: "Le thème est appliqué immédiatement, et le choix de l'appareil sert aux futures connexions."
                        color: Qt.rgba(appColors.textColor.r, appColors.textColor.g, appColors.textColor.b, 0.6)
                        wrapMode: Text.WordWrap
                        font.pixelSize: 13
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
