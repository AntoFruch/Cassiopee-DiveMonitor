import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "pages" as Pages
import "overlay" as Overlay

ApplicationWindow {
    visible: true
    width: 400
    height: 750
    title: "Subsurface"

    required property var builtInStyles

    // ── Colors ──────────────────────────────────────────────────

    readonly property bool darkMode: Qt.styleHints.colorScheme === Qt.Dark

    readonly property color bgColor : darkMode ? "#000000" : "#FFFFFF"
    readonly property color textColor : darkMode ? "#FFFFFF" : "#000000"
    readonly property color listButtonColor : darkMode ?  "#333333" : "#eeeeee"
    readonly property color separatorColor : darkMode ? "#1C1C1C" : "#CCCCCC"
    readonly property color accentColor: "#1981BD"

    QtObject {
        id: deviceState

        property string selectedBrand: ""
        property string selectedModel: ""
        property string selectedConnectionMode: ""
        property bool isDeviceConnected: dcWrapper.connected

        readonly property var devices: dcWrapper ? dcWrapper.supportedDevices : [];

        readonly property string currentDeviceName: selectedBrand + " " + selectedModel
        readonly property string currentDeviceDisplay: currentDeviceName + " | " + selectedConnectionMode

        function modelsForBrand(brand) {
            let entry = devices.find(d => d.vendor === brand)
            if (!entry)
                return []

            return entry.products.map(p => p.name)
        }

        function connectionModesFor(brand, model) {
            let entry = devices.find(d => d.vendor === brand)
            if (!entry)
                return []

            let product = entry.products.find(p => p.name === model)
            if (!product)
                return []

            return product.transports
        }
    }


    // ── Root column ──────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Header bar ──────────────────────────────────────────────────────
        Overlay.Header{
            id: header
        }
        // ── Main display Space ──────────────────────────────────────────────────────
        StackView{
            id: stack
            Layout.fillWidth: true
            Layout.fillHeight: true
            onCurrentItemChanged: {
                header.title = currentItem.title
            }

            initialItem: Pages.DiveList {
            }
        }
    }

    // ──  "+" button ───────────────────────────────────────────
    Overlay.AddDiveButton{}
}
