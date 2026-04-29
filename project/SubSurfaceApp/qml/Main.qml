import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCore

import "pages" as Pages
import "overlay" as Overlay

ApplicationWindow {
    id: appWindow
    visible: true
    width: 400
    height: 750
    title: "Subsurface"

    readonly property bool darkMode: appColors.darkMode
    readonly property color bgColor: appColors.bgColor
    readonly property color textColor: appColors.textColor
    readonly property color listButtonColor: appColors.listButtonColor
    readonly property color separatorColor: appColors.separatorColor
    readonly property color accentColor: appColors.accentColor
    readonly property bool hideHeaderForCurrentPage: !!stack.currentItem && !!stack.currentItem.hideHeader

    Settings {
        id: userPrefs
        property var theme: Qt.Unknown

        property string savedBrand
        property string savedModel
        property string savedConnectionMode
        property string savedPort
    }

    // ── Colors ──────────────────────────────────────────────────
    QtObject {
        id: appColors

        readonly property bool followSystem: userPrefs.theme === Qt.Unknown
        readonly property bool darkMode: followSystem
                                         ? Qt.styleHints.colorScheme === Qt.Dark
                                         : userPrefs.theme === Qt.Dark

        readonly property color bgColor: darkMode ? "#000000" : "#FFFFFF"
        readonly property color textColor: darkMode ? "#FFFFFF" : "#000000"
        readonly property color listButtonColor: darkMode ? "#333333" : "#eeeeee"
        readonly property color separatorColor: darkMode ? "#1C1C1C" : "#CCCCCC"
        readonly property color accentColor: "#1981BD"
    }

    QtObject {
        id: deviceState

        property string selectedBrand: userPrefs.savedBrand
        property string selectedModel: userPrefs.savedModel
        property string selectedConnectionMode: userPrefs.savedConnectionMode
        property string selectedPort: userPrefs.savedPort
        property bool isDeviceConnected: dcWrapper.connected

        readonly property var devices: dcWrapper ? dcWrapper.supportedDevices : [];

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
            visible: !appWindow.hideHeaderForCurrentPage
            Layout.preferredHeight: visible ? implicitHeight : 0
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
