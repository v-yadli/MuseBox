// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
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

        Button {
            id: addtrack
            x: 16
            y: 28

            normalImage: "AddTrack.png"
            pushedImage: "AddTrackHL.png"

            onReleased:{
                trackView.addTrack()
            }
        }

        Button {
            id: setting
            x: 750
            y: 28
            width: 51
            height: 26

            onReleased:{
                musebox.openConfigurationDialog()
            }
        }

        Button {
            id: saveProjectButton
            x: 816
            y: 28
            width: 70
            height: 70

            onReleased:{
                main.saveProject()
            }
        }

        Button {
            id: resetAudio
            x: 750
            y: 72
            width: 51
            height: 26

            onReleased: {
                musebox.resetAudio()
            }
        }

        Button {
            id: newProjectButton
            x: 898
            y: 28
            width: 50
            height: 70

            onReleased:{
                main.newProject()
            }
        }

        Button {
            id: loadProjectButton
            x: 964
            y: 28
            width: 50
            height: 70

            onReleased:{
                main.loadProject()
            }
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
        Button {
            id: rw
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 10

            normalImage: "RW.png"
            pushedImage: "RWHL.png"

            onPressed: {
                musebox.setSpeed(-2.0)
                musebox.moveStart()
            }

            onReleased: {
                musebox.setSpeed(1.0)
                musebox.moveEnd()
            }

            z: 1
        }
        Button {
            id: ff
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 60
            x: 87
            y: 40
            z: 2

            normalImage: "FF.png"
            pushedImage: "FFHL.png"

            onPressed:{
                musebox.setSpeed(2.0)
                musebox.moveStart()
            }

            onReleased:{
                musebox.setSpeed(1.0)
                musebox.moveEnd()
            }

        }

        Button {
            id: play
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 110

            x: 187
            y: 40
            z: 4

            normalImage: "Play.png"
            pushedImage: "PlayHL.png"
            latch: true
            onPushed:{
                musebox.play()
            }
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
        Button {
            id: stop
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 210
            x: 137
            y: 40
            z: 6

            normalImage: "Stop.png"
            pushedImage: "StopHL.png"
            onReleased:{
                musebox.stop()
                play.release()
            }
        }
        Button {
            id: click
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 260
            x: 137
            y: 40
            z: 7

            latch: true

            onPushed: {
                musebox.setClick(true)
            }
            onReleased:{
                musebox.setClick(false)
            }
        }
        Button {
            id: loop
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 310
            y: 40
            z: 8

            latch: true

            onPushed: {
                musebox.setLoop(true)
            }
            onReleased:{
                musebox.setLoop(false)
            }
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
