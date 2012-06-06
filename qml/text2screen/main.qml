import QtQuick 1.1

Rectangle {
    id: mainRectangle
    width: 800
    height: 600

    property int charsPerSecond: 10
    property bool moveFast: false

    Flickable {
        id: flickArea
        anchors.fill: parent
        contentWidth: textItem.width; contentHeight: textItem.height
        flickableDirection: Flickable.VerticalFlick
        interactive: true
        clip: true
        focus: true

        Text {
            id: textItem
            wrapMode: Text.Wrap
            width: mainRectangle.width

            text: helper.getSpeechText()
            anchors.centerIn: parent
            font.pixelSize: 60

            Component.onCompleted: goTonitialPosition()

            onWidthChanged: goTonitialPosition();
            onHeightChanged: goTonitialPosition();

            function goTonitialPosition() {
                mainRectangle.moveFast = true;
                flickArea.contentY = (-2/3)*flickArea.height;
            }
        }

        Keys.onPressed: {
            if ( event.key == Qt.Key_F) {
                helper.toggleFullScreen();
            }
            if ( event.key == Qt.Key_Plus) {
                textItem.font.pixelSize *= 1.2;
            }
            if ( event.key == Qt.Key_Minus) {
                textItem.font.pixelSize *= 1/1.2;
            }

            if ( event.key == Qt.Key_0) {
                textItem.goTonitialPosition();
            }

            console.debug("event.key == Qt.Key_Space " + (event.key == Qt.Key_Space));

            if ( event.key == Qt.Key_Space) {
                if (scrollAnimation.running) {
                    flickArea.contentY = flickArea.contentY;
                } else {
                    flickArea.contentY = textItem.height - flickArea.height;
                }
            }
        }

        Behavior on contentY {
            SmoothedAnimation {
                id: scrollAnimation
                easing.type: Easing.Linear
                maximumEasingTime: 1
                velocity: mainRectangle.moveFast ? 10000 : (mainRectangle.charsPerSecond * textItem.font.pixelSize) / (mainRectangle.width / textItem.font.pixelSize)
                onVelocityChanged: console.debug("vel = " + scrollAnimation.velocity)
                onRunningChanged: {
                    if (!running) {
                        mainRectangle.moveFast = false;
                    }
                }
            }
        }
    }
}
