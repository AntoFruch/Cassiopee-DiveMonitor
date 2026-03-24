import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import QtGraphs

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    required property var dive
    property string title: dive.name
}
