/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/





import QtQuick 2.1

Item {
    id: root
    width: ListView.view.width
    height: units.gu(100)

    property alias text: textitem.text
    property bool showArrow: true
    signal clicked

    property int count: ListView.view.count
    property variant section: ListView.section
    property variant nextSection: ListView.nextSection
    property variant previousSection: ListView.previousSection

    Rectangle {
        anchors.fill: parent
        color: mouse.pressed ? "#d9d9d9" : "white"
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#d1d0d5"
        visible: model.index == 0 || section != previousSection
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#d1d0d5"
        visible: model.index == count || (section != nextSection && model.index)
    }

    Text {
        id: textitem
        color: "black"
        font.pixelSize: units.gu(45)
        text: modelData
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: units.gu(100)
        anchors.right: arrow.left
        elide: Text.ElideMiddle
    }

    Rectangle {
        anchors.left: textitem.left
        anchors.right: parent.right
        anchors.margins: units.gu(15)
        height: 1
        color: "#d1d0d5"
        visible: model.index > 0
    }

    Image {
        id: arrow
        anchors.right: parent.right
        anchors.rightMargin: units.gu(20)
        anchors.verticalCenter: parent.verticalCenter
        source: "images/navigation_next_item.png"
        visible: root.showArrow
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        onClicked: root.clicked()
    }

    /*
    transitions: Transition {
        NumberAnimation {
            properties: "scale,opacity"
            duration: 50
        }
    }

    states: State {
        name: "pressed"
        when: mouse.pressed

        PropertyChanges {
            target: textitem
            scale: 0.9
            opacity: 0.6
        }
    }
    */
}
