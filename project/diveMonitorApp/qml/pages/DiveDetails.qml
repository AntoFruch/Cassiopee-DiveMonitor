import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "../components"

Rectangle {
    id: root
    color: appColors.bgColor

    required property var dive
    property string title: dive.date

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        DiveGraphView {
            id: diveGraph
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: parent.height * 0.5
            dive: root.dive
            showModeSelector: true
            overlayModeSelector: false
            showExpandButton: true
            onExpandRequested: {
                stack.push("DiveGraphPage.qml", {
                    "dive": root.dive,
                    "initialDisplayMode": diveGraph.displayMode
                })
            }
        }

        // Séparateur
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: appColors.separatorColor
        }

        // Panneau d'infos scrollable
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: availableWidth

            ColumnLayout {
                width: parent.width
                spacing: 0

                // Section : Informations générales
                SectionHeader {
                    Layout.fillWidth: true
                    text: qsTr("Informations générales")
                }

                InfoRow {
                    Layout.fillWidth: true
                    label: qsTr("Date")
                    value: dive.date
                    iconSource: "calendar"
                }
                InfoRow {
                    Layout.fillWidth: true
                    label: qsTr("Durée")
                    value: Number(dive.diveTime / 60).toFixed(0) + " min"
                    iconSource: "clock"
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: appColors.separatorColor
                    Layout.leftMargin: 16
                }

                // Section : Profondeurs
                SectionHeader {
                    Layout.fillWidth: true
                    text: qsTr("Profondeurs")
                }

                GridLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    Layout.bottomMargin: 8
                    columns: 2
                    columnSpacing: 12
                    rowSpacing: 12

                    StatCard {
                        Layout.fillWidth: true
                        label: qsTr("Prof. max.")
                        value: dive.maxDepth.toFixed(1) + " m"
                        accent: true
                    }
                    StatCard {
                        Layout.fillWidth: true
                        label: qsTr("Prof. moy.")
                        value: dive.avgDepth.toFixed(1) + " m"
                        accent: false
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: appColors.separatorColor
                    Layout.leftMargin: 16
                }

                // Section : Températures
                SectionHeader {
                    Layout.fillWidth: true
                    text: qsTr("Températures")
                }

                GridLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    Layout.bottomMargin: 8
                    columns: 3
                    columnSpacing: 12
                    rowSpacing: 12

                    StatCard {
                        Layout.fillWidth: true
                        label: qsTr("Surface")
                        value: dive.surfaceTemp.toFixed(1) + " °C"
                    }
                    StatCard {
                        Layout.fillWidth: true
                        label: qsTr("Min.")
                        value: dive.minTemp.toFixed(1) + " °C"
                    }
                    StatCard {
                        Layout.fillWidth: true
                        label: qsTr("Max.")
                        value: dive.maxTemp.toFixed(1) + " °C"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: appColors.separatorColor
                    Layout.leftMargin: 16
                }

                // Section : Conditions
                SectionHeader {
                    Layout.fillWidth: true
                    text: qsTr("Conditions")
                }

                InfoRow {
                    Layout.fillWidth: true
                    label: qsTr("Pression atmos.")
                    value: dive.atmosPressure + " mbar"
                    iconSource: "gauge"
                }

                // Padding bas
                Item { Layout.preferredHeight: 24 }
            }
        }
    }

    // Composant : en-tête de section
    component SectionHeader: Rectangle {
        property alias text: label.text
        height: 36
        color: appColors.listButtonColor

        Text {
            id: label
            anchors {
                left: parent.left
                leftMargin: 16
                verticalCenter: parent.verticalCenter
            }
            font.pixelSize: 12
            font.weight: Font.Medium
            color: appColors.accentColor
            text: ""
        }
    }

    // Composant : ligne label / valeur avec séparateur
    component InfoRow: Rectangle {
        property string label: ""
        property string value: ""
        property string iconSource: ""

        height: 48
        color: "transparent"

        RowLayout {
            anchors {
                fill: parent
                leftMargin: 16
                rightMargin: 16
            }
            spacing: 10

            Text {
                text: label
                font.pixelSize: 15
                color: appColors.textColor
                opacity: 0.6
                Layout.fillWidth: true
            }
            Text {
                text: value
                font.pixelSize: 15
                font.weight: Font.Medium
                color: appColors.textColor
                horizontalAlignment: Text.AlignRight
            }
        }

        // Séparateur interne
        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                leftMargin: 16
            }
            height: 1
            color: appColors.separatorColor
        }
    }

    // Composant : carte statistique
    component StatCard: Rectangle {
        property string label: ""
        property string value: ""
        property bool accent: false

        height: 64
        radius: 10
        color: accent ? Qt.rgba(
            Qt.color(appColors.accentColor).r,
            Qt.color(appColors.accentColor).g,
            Qt.color(appColors.accentColor).b,
            appColors.darkMode ? 0.25 : 0.1
        ) : appColors.listButtonColor

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 4

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: label
                font.pixelSize: 11
                font.weight: Font.Medium
                color: accent ? appColors.accentColor : appColors.textColor
                opacity: accent ? 1.0 : 0.5
            }
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: value
                font.pixelSize: 20
                font.weight: Font.Medium
                color: accent ? appColors.accentColor : appColors.textColor
            }
        }
    }
}
