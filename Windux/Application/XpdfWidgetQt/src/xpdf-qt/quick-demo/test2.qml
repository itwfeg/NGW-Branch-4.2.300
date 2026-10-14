import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.0

Window {
    visible: true
    width: 800
    height: 800
    title: "test2"

    SplitView {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: openBtn.top
        anchors.bottomMargin: 8
        orientation: Qt.Horizontal

        TreeView {
            id: outline
            model: xpdf.getOutlineModel()
            TableViewColumn {
                role: "display"
            }
            onClicked: {
                xpdf.gotoOutlineTarget(index)
            }
        }

        Xpdf {
            id: xpdf
            Layout.minimumWidth: 200
            Layout.fillWidth: true
            zoom: slider.value
            // onPageChange: { pageNumber.text = pageNum; }
            onMidPageChange: { pageNumber.text = pageNum; }
        }
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
        anchors.right: pageNumber.left
        anchors.bottom: parent.bottom
        anchors.margins: 8
        onClicked: {
            xpdf.find(findText.text, 0);
        }
    }

    Text {
        id: pageNumber
        text: ""
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 8
    }

    FileDialog {
        id: openDlg
        title: "Open PDF file"
        onAccepted: {
            xpdf.src = openDlg.fileUrl.toString();
            setOutlineOpenItems(undefined)
        }
        function setOutlineOpenItems(idx) {
            if (idx && idx.valid) {
                if (!xpdf.getOutlineStartsOpen(idx)) {
                    return;
                }
                outline.expand(idx);
            }
            var n = outline.model.rowCount(idx);
            for (var i = 0; i < n; ++i) {
                setOutlineOpenItems(outline.model.index(i, 0, idx));
            }
        }
    }
}
