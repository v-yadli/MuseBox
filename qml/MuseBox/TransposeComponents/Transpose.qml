// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: mainframe
    width: 1024
    height: 110
    opacity: 1

    function updateTimeAndBeat(min,sec,mil,bar,beat)
    {
        timeIndicator.updateTimeAndBeat(min,sec,mil,bar,beat)
    }

    function togglePlay()
    {
        if(play.state == "Pushed")
        {
            stop.triggerStop()
        }else
        {
            play.triggerPlay()
        }
    }

    function updateMasterDbMeter(l,r)
    {
        dbMeter.update(l,r)
    }
    Rectangle {
        id: seperator
        height: 16
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        z: 6
        anchors.top: parent.top
        anchors.topMargin: 0
        gradient: Gradient {
            GradientStop {
                position: 0.260
                color: "#000000"
            }

            GradientStop {
                position: 0.790
                color: "#3e3e3e"
            }
        }
    }

    Image {
        id: bg
        x: 0
        y: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
        fillMode: Image.Tile
        source: "../navy_blue.png"

        AddTrack {
            id: addtrack
            x: 16
            y: 28
        }
    }

    Rectangle {
        id: transposeControl
        x: 86

        y: 21
        width: 655
        height: 83
        radius: 12
        anchors.left: parent.left
        anchors.leftMargin: 86
        smooth: true
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#ffffff"
            }

            GradientStop {
                position: 1
                color: "#999999"
            }
        }
        RW {
            id: rw
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 10
            z: 1
        }
        FF {
            id: ff
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 60
            x: 87
            y: 40
            z: 2
        }

        PLAY {
            id: play
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 110

            x: 187
            y: 40
            z: 4
        }
        RECORD {
            id: record

            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 160
            x: 237
            y: 40
            z: 5
        }
        STOP {
            id: stop
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 210
            x: 137
            y: 40
            z: 6
        }
        ClickButton {
            id: click
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 260
            x: 137
            y: 40
            z: 7
        }
        LoopButton {
            id: loop
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 310
            y: 40
            z: 8
        }

        TimeIndicator {

            id: timeIndicator
            x: 370
            y: 10
        }

        HorizonDbMeter {
            id: dbMeter
            x: 484
            y: 10
        }

    }
}
