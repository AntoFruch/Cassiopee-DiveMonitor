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

    // ── Hardcoded dive list | expected to be deleted soon ────────────────────────────────
    ListModel {
        id: diveModel
        ListElement { name: "Cool Dive";        date: "10/10/2026" }
        ListElement { name: "Blue Hole";        date: "12/10/2026" }
        ListElement { name: "Coral Garden";     date: "15/10/2026" }
        ListElement { name: "Shark Point";      date: "18/10/2026" }
        ListElement { name: "The Wall";         date: "20/10/2026" }
        ListElement { name: "Night Dive";       date: "22/10/2026" }
        ListElement { name: "Wreck Div";        date: "25/10/2026" }
        ListElement { name: "Deep Blue";        date: "28/10/2026" }
        ListElement { name: "Cave Dive";        date: "01/11/2026" }
        ListElement { name: "Drift Dive";       date: "05/11/2026" }
        ListElement { name: "Manta Point";      date: "08/11/2026" }
        ListElement { name: "Turtle Bay";       date: "10/11/2026" }
        ListElement { name: "Cool Dive 2";      date: "11/10/2026" }
        ListElement { name: "Blue Hole 2";      date: "13/10/2026" }
        ListElement { name: "Coral Garden 2";   date: "16/10/2026" }
        ListElement { name: "Shark Point 2";    date: "19/10/2026" }
        ListElement { name: "The Wall 2";       date: "21/10/2026" }
        ListElement { name: "Night Dive 2";     date: "23/10/2026" }
        ListElement { name: "Wreck Div 2";      date: "26/10/2026" }
        ListElement { name: "Deep Blue 2";      date: "29/10/2026" }
        ListElement { name: "Cave Dive 2";      date: "02/11/2026" }
        ListElement { name: "Drift Dive 2";     date: "06/11/2026" }
        ListElement { name: "Manta Point 2";    date: "09/11/2026" }
        ListElement { name: "Turtle Bay 2";     date: "11/11/2026" }
        ListElement { name: "Cool Dive 3";      date: "12/10/2026" }
        ListElement { name: "Blue Hole 3";      date: "14/10/2026" }
        ListElement { name: "Coral Garden 3";   date: "17/10/2026" }
        ListElement { name: "Shark Point 3";    date: "20/10/2026" }
        ListElement { name: "The Wall 3";       date: "22/10/2026" }
        ListElement { name: "Night Dive 3";     date: "24/10/2026" }
        ListElement { name: "Wreck Div 3";      date: "27/10/2026" }
        ListElement { name: "Deep Blue 3";      date: "30/10/2026" }
        ListElement { name: "Cave Dive 3";      date: "03/11/2026" }
        ListElement { name: "Drift Dive 3";     date: "07/11/2026" }
        ListElement { name: "Manta Point 3";    date: "10/11/2026" }
        ListElement { name: "Turtle Bay 3";     date: "12/11/2026" }
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
                diveModel: diveModel
            }
        }
    }

    // ──  "+" button ───────────────────────────────────────────
    Overlay.AddDiveButton{}
}
