// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "TransposeComponents"

Rectangle {
    id: base
    width: 200
    height: 80

    signal selected()
    signal setTrackName(string newText)

    MouseArea{
        anchors{
            top:tName.bottom
            bottom:parent.bottom
            left:parent.left
            right:parent.right
        }
        onClicked:{
            if(tNameEdit.visible)
            {
                tNameEdit.visible = false
                tName.visible = true
                setTrackName(tNameEdit.text)
            }
            selected()
        }
    }

    property string trackName : ""
    onTrackNameChanged:
    {
        tName.text=trackName
    }

    gradient: Gradient {
        GradientStop {
            position: 0.090
            color: "#2b2b2b"
        }

        GradientStop {
            position: 0.960
            color: "#404040"
        }
    }

    function updateDbMeter(l,r)
    {
        dbMeter.update(l,r)
    }

    TextEdit{
        id: tNameEdit
        height: 20
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        visible: false
        anchors.fill: parent
        color: "#e2cbad"
        wrapMode: TextEdit.WordWrap
        font.bold: true
        font.pixelSize: 14
    }

    Text {
        MouseArea{
            anchors.fill: parent
            onClicked: {
                tNameEdit.text = parent.text
                tNameEdit.focus = true
                tNameEdit.visible = true
                tNameEdit.enabled = true
                parent.visible = false
            }
        }
        id: tName
        height: 20
        color: "#db9c44"
        text: qsTr("")
        wrapMode: TextEdit.NoWrap
        font.bold: true
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        font.pixelSize: 14
    }
/*
    Image {
        id: recordButton
        x: 240
        width: 60
        anchors.top: trackName.bottom
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        source: "qrc:/qtquickplugin/images/template_image.png"
    }
*/
    HorizonDbMeter{
        id:dbMeter
        x: 13
        y: 25
    }
/*
    Image {
        id: trackIcon
        x: 0
        y: 20
        width: 60
        anchors.top: trackName.bottom
        anchors.topMargin: 0
        source: "qrc:/qtquickplugin/images/template_image.png"
        anchors.bottom: parent.bottom
        anchors.rightMargin: 240
        anchors.bottomMargin: 0
        anchors.right: parent.right
    }
    TODO work out the image sources
    */
}
