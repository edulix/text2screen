import QtQuick 1.1

Rectangle {
    id: mainRectangle
    width: 800
    height: 600

    property int charsPerSecond: 18
    property bool moveFast: false

    property real charWidth: 0

    Rectangle {
        id: clockRect
        clip: true
        anchors.bottom: mainRectangle.bottom
        anchors.right: mainRectangle.right
        width: 100
        height: 40

        Text {
            id: clockText
            anchors.centerIn: parent
            font {
                pixelSize: 20
            }

            property int minutes: 0
            property int seconds: 0
            property int timerSeconds: 0
            text: "00:00"



            function timeChanged() {
                timerSeconds = timerSeconds + 1;
                seconds = timerSeconds % 60;

                if (timerSeconds % 60 == 0) {
                    minutes++;
                }

                clockText.text = checkTime(minutes) + ":" + checkTime(seconds);
            }

            function checkTime(i) {
                return (i<10) ? "0"+i : i;
            }
        }

        Timer {
            id: checkingTimer
            interval: 1000
            repeat: true
            running: countTimer.running
            onTriggered: clockText.timeChanged()
        }



        Timer {
            id: countTimer
            running: false
            interval: 1000000000
        }
    }

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
            font {
                pixelSize: 60
            }

            onFontChanged: calcPixelWidth()
            onTextChanged: calcPixelWidth()

            function calcPixelWidth()
            {
                var sampleText = "En un lugar de la Mancha, de cuyo nombre no quiero acordarme";
                var textElement = Qt.createQmlObject('import Qt 4.7; Text { font.pixelSize:' + font.pixelSize + '; text: "' + sampleText + '"}',
                    textItem, "calcTextWidth");
                mainRectangle.charWidth = textElement.width / sampleText.length;
                textElement.destroy();

            }

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
                countTimer.stop();
            }

            console.debug("event.key == Qt.Key_Space " + (event.key == Qt.Key_Space));

            if ( event.key == Qt.Key_Space) {
                if (scrollAnimation.running) {
                    flickArea.contentY = flickArea.contentY;
                    countTimer.pause();
                } else {
                    flickArea.contentY = textItem.height - flickArea.height;
                    countTimer.start();
                }
            }
        }

        Behavior on contentY {
            SmoothedAnimation {
                id: scrollAnimation
                easing.type: Easing.Linear
                maximumEasingTime: 1
                velocity: mainRectangle.moveFast ? 10000 : (mainRectangle.charsPerSecond * textItem.font.pixelSize) / (mainRectangle.width / mainRectangle.charWidth)
                onVelocityChanged: console.debug("vel = " + scrollAnimation.velocity + " (mainRectangle.width / mainRectangle.charWidth) = " + (mainRectangle.width / mainRectangle.charWidth))
                onRunningChanged: {
                    if (!running) {
                        mainRectangle.moveFast = false;
                    }
                }
            }
        }
    }
}
