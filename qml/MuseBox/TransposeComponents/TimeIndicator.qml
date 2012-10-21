// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    function updateTimeAndBeat(min,sec,mil,bar,beat)
    {
        timeText.text = min+":"+sec+"."+mil
        beatText.text = bar + "/" + beat
    }

    width: 100
    height: 62
    clip: true
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#000000"
        }

        GradientStop {
            position: 0.950
            color: "#515151"
        }
    }

    Text {
        id: timeText
        x: 6
        y: 6
        width: 83
        height: 26
        color: "#4ea631"
        text: qsTr("")
        smooth: true
        styleColor: "#ffffff"
        font.family: "Courier New"
        verticalAlignment: Text.AlignTop
        font.pixelSize: 21
    }

    Text {
        id: beatText
        x: 6
        y: 32
        width: 83
        height: 25
        color: "#4ea631"
        text: qsTr("")
        smooth: true
        font.pixelSize: 21
        styleColor: "#ffffff"
        font.family: "Courier New"
        verticalAlignment: Text.AlignTop
    }
}
