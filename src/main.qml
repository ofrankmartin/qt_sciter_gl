import QtQuick 2.12
import QtQuick.Window 2.12

import QtQuick 2.0
import OpenGLUnderQML 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Qt + OpenGL + Sciter")

    SciterItem {
        id: sciter
        anchors.fill: parent
        HoverHandler {}
    }
}
