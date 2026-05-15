import QtQuick
import QtQuick.Layouts
import "../components"

ColumnLayout {
    id: root

    // On récupère la plongée sélectionnée dans la liste.
    // Les points ne sont pas embarqués dans cet objet pour éviter de l'alourdir.
    required property var dive
    property string title: dive.date
    DiveGraphView {
        id: diveGraph

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        Layout.preferredHeight: root.height * 0.5

        dive: root.dive
        accentColorValue: appColors.accentColor
        backgroundColorValue: appColors.bgColor
        textColorValue: appColors.textColor
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
}
