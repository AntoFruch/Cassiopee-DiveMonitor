import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls
import DiveMonitorCustom 1.0

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    // Logo en fond, centré, semi-transparent
    Image {
        anchors.centerIn: parent
        source: "qrc:icon/icon"
        width: parent.width * 0.6
        height: width
        fillMode: Image.PreserveAspectFit
        opacity: 0.07
    }

    ListView {
        property string title: "Saved Dives"
        id: listView
        anchors.fill: parent
        clip: true
        bottomMargin: 80
        spacing: 0
        model: DiveListModel {
            id: diveListModel
            Component.onCompleted: loadDives()
        }

        delegate: Item {
            width: listView.width
            height: 60
            Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                height: 60
                color: appColors.listButtonColor
                border.width: 0
                radius: 0
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: appColors.separatorColor
                }
                Text {
                    anchors {
                        left: parent.left
                        leftMargin: 14
                        verticalCenter: parent.verticalCenter
                    }
                    text: model.date + " | Dive time :  " + model.diveTime + "s"
                    font.pixelSize: 16
                    font.bold: true
                    color: appColors.textColor
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        stack.push("DiveDetails.qml", {"dive": model})
                    }
                }
            }
        }
    }
}