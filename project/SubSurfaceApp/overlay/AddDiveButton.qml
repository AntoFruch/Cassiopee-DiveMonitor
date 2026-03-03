import QtQuick 2.15

Rectangle {
    id: addDiveBtn
    width: 60
    height: 60
    radius: 30
    color: accentColor
    anchors {
        horizontalCenter: parent.horizontalCenter
        bottom: parent.bottom
        bottomMargin: 24
    }

    visible: stack.depth <= 1

    Image {
        anchors.centerIn: parent
        height: parent.height * 3 / 4

        source: "qrc:/images/plus"
        fillMode: Image.PreserveAspectFit
    }

    MouseArea {
        anchors.fill: parent
        onClicked: stack.push("../pages/AddDives.qml")
    }
}
