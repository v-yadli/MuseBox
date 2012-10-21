// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "TransposeComponents"

Rectangle {
    id: base
    width: 300
    height: 80

    property string trackName:""
    onTrackNameChanged: {
        console.log("reaching onTrackNameChanged");
        trackNameTextEdit.text=trackName
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

    TextEdit {
        onTextChanged: {
            trackName=text
        }
        id: trackNameTextEdit
        height: 20
        color: "#db9c44"
        text: qsTr("Track Name")
        wrapMode: TextEdit.WordWrap
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
        x: 75
        y: 40
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
