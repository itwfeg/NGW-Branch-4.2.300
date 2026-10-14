import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.0

Window {
    visible: true
    width: 800
    height: 800
    title: "test1"

    Xpdf {
        id: xpdf
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: openBtn.top
        anchors.bottomMargin: 8
        zoom: slider.value
    }

    Button {
        id: openBtn
        text: "Open"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 8
        onClicked: {
            openDlg.open();
        }
    }

    Slider {
        id: slider
        width: 200
        minimumValue: 25
        maximumValue: 400
        value: 100
        anchors.left: openBtn.right
        anchors.verticalCenter: openBtn.verticalCenter
        anchors.margins: 8
    }

    Text {
        id: zoomValue
        text: xpdf.zoom
        anchors.left: slider.right
        anchors.verticalCenter: slider.verticalCenter
        anchors.margins: 8
    }

    TextField {
        id: findText
        anchors.left: zoomValue.right
        anchors.right: findBtn.left
        anchors.bottom: parent.bottom
        anchors.margins: 8
    }

    Button {
        id: findBtn
        text: "Find"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 8
        onClicked: {
            xpdf.find(findText.text, 0);
            if (xpdf.hasSelection()) {
              xpdf.scrollToCentered(xpdf.getCurrentSelectionPage(),
                                    0.5 * (xpdf.getCurrentSelectionX0() + xpdf.getCurrentSelectionX1()),
                                    0.5 * (xpdf.getCurrentSelectionY0() + xpdf.getCurrentSelectionY1()));
            }
        }
    }

    FileDialog {
        id: openDlg
        title: "Open PDF file"
        onAccepted: {
            xpdf.src = openDlg.fileUrl.toString();
        }
    }
}
