import QtQuick 1.1

Rectangle {
    width: 360
    height: 360
    Text {
        id: textItem
        text: qsTr("Click here to open the speech..")
        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            textItem.text = helper.getSpeechText();
        }
    }
}
