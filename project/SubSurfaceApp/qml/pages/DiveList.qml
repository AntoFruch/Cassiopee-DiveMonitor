import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls

ListView {
    property string title: "Saved Dives"
    required property var diveModel

    id: listView
    Layout.fillWidth: true
    Layout.fillHeight: true
    clip: true
    bottomMargin: 80   // leave room so FAB doesn't hide last item
    spacing: 0

    model: diveModel

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
            color: listButtonColor
            border.width: 0    // pas de bordure
            radius: 0          // pas d'arrondi

            // Séparateur en bas
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: separatorColor
            }

            Text {
                anchors {
                    left: parent.left
                    leftMargin: 14
                    verticalCenter: parent.verticalCenter
                }
                text: model.name + " | " + model.date
                font.pixelSize: 16
                font.bold: true
                color: textColor
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
