import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Window
import "../components"

Rectangle {
    id: root

    required property var dive
    property int initialDisplayMode: 0
    property string title: dive.date
    property bool hideSettingsButton: true
    readonly property bool isLandscape: Window.width > Window.height
    readonly property bool hideHeader: isLandscape

    color: bgColor

    DiveGraphView {
        id: fullGraph
        anchors.fill: parent
        dive: root.dive
        accentColorValue: accentColor
        backgroundColorValue: bgColor
        textColorValue: textColor
        displayMode: root.initialDisplayMode
        showModeSelector: true
        overlayModeSelector: root.isLandscape
        showExpandButton: false
    }

    Button {
        visible: root.isLandscape
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 12
        text: "Retour"
        z: 2

        onClicked: stack.pop()
    }
}
