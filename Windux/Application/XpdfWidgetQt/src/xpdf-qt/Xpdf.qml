//========================================================================
//
// Xpdf.qml
//
// Copyright 2016 Glyph & Cog, LLC
//
//========================================================================

import QtQuick 2.4
import Xpdf 4.1

Flickable {
    //~ XpdfItem doesn't currently know how to handle the case where
    //~   flickX/Y are out of bounds
    boundsBehavior: Flickable.StopAtBounds

    // properties passed through to XpdfItem
    property string src
    property double zoom: 100
    property color selectionColor: "#8080ff"

    // functions passed through to XpdfItem
    function loadFile(fileName) { return xpdfItem.loadFile(fileName); }
    function loadFileWithPassword(fileName, password)
        { return xpdfItem.loadFileWithPassword(fileName, password); }
    function getNumPages() { return xpdfItem.getNumPages(); }
    function getCurrentPage() { return xpdfItem.getCurrentPage(); }
    function getMidPage() { return xpdfItem.getMidPage(); }
    function gotoPage(pageNum) { xpdfItem.gotoPage(pageNum); }
    function gotoNextPage() { xpdfItem.gotoNextPage(); }
    function gotoPreviousPage() { xpdfItem.gotoPreviousPage(); }
    function scrollToCentered(page, x, y)
        { xpdfItem.scrollToCentered(page, x, y); }
    function hasSelection() { return xpdfItem.hasSelection(); }
    function getCurrentSelectionPage()
        { return xpdfItem.getCurrentSelectionPage(); }
    function getCurrentSelectionX0()
        { return xpdfItem.getCurrentSelectionX0(); }
    function getCurrentSelectionY0()
        { return xpdfItem.getCurrentSelectionY0(); }
    function getCurrentSelectionX1()
        { return xpdfItem.getCurrentSelectionX1(); }
    function getCurrentSelectionY1()
        { return xpdfItem.getCurrentSelectionY1(); }
    property int findBackward:      0x00000001
    property int findCaseSensitive: 0x00000002
    property int findNext:          0x00000004
    property int findOnePageOnly:   0x00000008
    property int findWholeWord:     0x00000010
    function find(s, flags) { return xpdfItem.find(s, flags); }
    function getOutlineModel() { return xpdfItem.getOutlineModel(); }
    function getOutlineTitle(index)
        { return xpdfItem.getOutlineTitle(index); }
    function getOutlineStartsOpen(index)
        { return xpdfItem.getOutlineStartsOpen(index); }
    function getOutlineTargetPage(index)
        { return xpdfItem.getOutlineTargetPage(index); }
    function gotoOutlineTarget(index)
        { xpdfItem.gotoOutlineTarget(index); }

    // signals passed through from XpdfItem
    signal pageChange(int pageNum)
    signal midPageChange(int pageNum)

    MouseArea {
        id: xpdfMouseArea
        x: parent.parent.contentX
        y: parent.parent.contentY
        width: parent.parent.width
        height: parent.parent.height
        z: 1
        acceptedButtons: Qt.AllButtons
        onClicked: {
            xpdfItem.mouseClick(mouse.button, mouse.x, mouse.y);
        }
    }

    PinchArea {
        id: xpdfPinchArea
        x: parent.parent.contentX
        y: parent.parent.contentY
        width: parent.parent.width
        height: parent.parent.height
        z: 1
        onPinchStarted: {
            xpdfItem.pinchStarted(pinch.center.x, pinch.center.y, pinch.scale);
        }
        onPinchUpdated: {
            xpdfItem.pinchUpdated(pinch.center.x, pinch.center.y, pinch.scale);
        }
    }

    XpdfItem {
        id: xpdfItem

        // properties passed through from the Flickable
        // NB: the XpdfItem is re-parented to Flickable.contentItem, so
        // we have to use parent.parent to get to the Flickable.
        flickX: parent.parent.contentX
        flickY: parent.parent.contentY
        flickWidth: parent.parent.width
        flickHeight: parent.parent.height
        src: parent.parent.src
        zoom: parent.parent.zoom
        selectionColor: parent.parent.selectionColor

        // values passed back out to the Flickable
        onUpdateFlickable: {
            parent.parent.contentWidth = w;
            parent.parent.contentHeight = h;
            parent.parent.contentX = x;
            parent.parent.contentY = y;
        }

        // signals passed back out to the Flickable
        onPageChange: { parent.parent.pageChange(pageNum); }
        onMidPageChange: { parent.parent.midPageChange(pageNum); }
    }
}
