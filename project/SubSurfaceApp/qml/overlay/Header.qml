import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: header
    Layout.fillWidth: true
    height: 70
    color: accentColor

    property var title

    Text {
        anchors.centerIn: parent
        text: title ? title : "SubSurface"
        font.pixelSize: 22
        font.bold: true
        color: "white"
    }


    // Settings Button
    Image {
        id: settingsBtn
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 16
        height: parent.height * 2 / 3

        source: "qrc:/images/settings"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onClicked: console.log("settings clicked")
        }
    }

    // Back Button
    Image {
        id: backBtn
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 16
        height: parent.height * 2 / 3

        source: "qrc:/images/back"
        fillMode: Image.PreserveAspectFit

        visible: stack.depth > 1

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (stack.depth > 1) {
                    stack.pop()
                }
            }
        }
    }
}
