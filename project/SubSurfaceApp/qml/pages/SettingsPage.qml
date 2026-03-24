import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: root

    required property var deviceState

    property string title: "Settings"
    property bool hideSettingsButton: true
    property var settingValues: ({
        "language": "English",
        "theme": Qt.styleHints.colorScheme === Qt.Dark ? "Night" : "Day",
        "depth": "Meter (m)",
        "pressure": "bar (bar)",
        "temperature": "Celsius (°C)",
        "velocity": "Meter per second (m/s)"
    })
    property var settingOptions: ({
        "device": ["Mares quad Air", "Device 2", "Device 3"],
        "language": ["English", "Francais"],
        "theme": ["Day", "Night"],
        "depth": ["Meter (m)", "Feet (ft)"],
        "pressure": ["bar (bar)", "psi (psi)"],
        "temperature": ["Celsius (°C)", "Fahrenheit (°F)"],
        "velocity": ["Meter per second (m/s)", "Miles per hour (mph)"]
    })
    property string currentSettingKey: ""
    property string popupTitle: ""
    property var currentOptions: []

    function settingValue(settingKey, fallbackValue) {
        if (settingKey === "device")
            return deviceState.currentDeviceDisplay

        return settingValues[settingKey] !== undefined ? settingValues[settingKey] : fallbackValue
    }

    function openSettingPopup(settingKey, titleText) {
        if (settingKey === "device") {
            devicePickerDialog.openDialog()
            return
        }

        currentSettingKey = settingKey
        popupTitle = titleText
        currentOptions = settingOptions[settingKey] || []
        settingsPopup.open()
    }

    function applySettingValue(value) {
        var updatedValues = Object.assign({}, settingValues)
        updatedValues[currentSettingKey] = value
        settingValues = updatedValues

        if (currentSettingKey === "theme") {
            Qt.styleHints.colorScheme = value === "Night" ? Qt.Dark : Qt.Light
        }

        settingsPopup.close()
    }

    Layout.fillWidth: true
    Layout.fillHeight: true
    clip: true

    background: Rectangle {
        color: bgColor
    }

    Column {
        width: root.availableWidth
        topPadding: 18
        bottomPadding: 24

        Repeater {
            model: [
                { type: "item", key: "device", label: "Set a device", popupTitle: "Choose device" },
                { type: "item", key: "language", label: "Language", popupTitle: "Choose language" },
                { type: "item", key: "theme", label: "Theme", popupTitle: "Choose theme" },
                { type: "section", label: "Unity parameter" },
                { type: "item", key: "depth", label: "Deepness/Distance", popupTitle: "Choose depth unit" },
                { type: "item", key: "pressure", label: "Pressure", popupTitle: "Choose pressure unit" },
                { type: "item", key: "temperature", label: "Temperature", popupTitle: "Choose temperature unit" },
                { type: "item", key: "velocity", label: "Velocity", popupTitle: "Choose velocity unit" }
            ]

            delegate: Item {
                width: root.availableWidth
                height: modelData.type === "section" ? 54 : 84

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: modelData.type === "section" ? 0 : 1
                    color: separatorColor
                }

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 10
                    anchors.rightMargin: 16
                    spacing: modelData.type === "section" ? 0 : 4

                    Text {
                        text: modelData.label
                        font.pixelSize: modelData.type === "section" ? 14 : 18
                        font.bold: true
                        color: modelData.type === "section" ? "#F4A321" : textColor
                    }

                    Text {
                        visible: modelData.type === "item"
                        text: root.settingValue(modelData.key, "")
                        font.pixelSize: 17
                        font.bold: true
                        color: "#A6A6A6"
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: modelData.type === "item"
                    z: 2
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: {
                        root.openSettingPopup(modelData.key, modelData.popupTitle)
                    }
                }
            }
        }
    }

    Popup {
        id: settingsPopup
        parent: Overlay.overlay
        x: Math.round((root.width - width) / 2)
        y: 150
        width: Math.min(root.width - 32, 260)
        modal: true
        focus: true
        padding: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            radius: 18
            color: "#F7F7F7"
            border.width: 2
            border.color: accentColor
        }

        contentItem: Column {
            spacing: 0
            clip: true

            Rectangle {
                width: settingsPopup.availableWidth
                height: 54
                color: "transparent"

                Text {
                    anchors.centerIn: parent
                    text: root.popupTitle
                    font.pixelSize: 18
                    font.bold: true
                    color: textColor
                }
            }

            Repeater {
                model: root.currentOptions

                delegate: ItemDelegate {
                    width: settingsPopup.availableWidth
                    height: 48
                    text: modelData
                    font.pixelSize: 16
                    font.bold: root.settingValue(root.currentSettingKey, "") === modelData
                    leftPadding: 18
                    rightPadding: 18

                    background: Rectangle {
                        radius: 12
                        color: "transparent"

                        Rectangle {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            anchors.topMargin: 3
                            anchors.bottomMargin: 3
                            radius: 10
                            color: highlighted || root.settingValue(root.currentSettingKey, "") === modelData ? "#E8EEFF" : "transparent"
                        }

                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: 1
                            visible: index < root.currentOptions.length - 1
                            color: "#D8D8D8"
                        }
                    }

                    onClicked: {
                        root.applySettingValue(modelData)
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
