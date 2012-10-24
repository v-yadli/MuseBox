// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 50
    height: 70
    radius: 0
    signal onPressed
    signal onReleased
    signal onPushed
    property string normalImage : "Stop.png"
    property string pushedImage : "StopHL.png"
    property bool latch : false
    function pushed(){
        return state == "Pushed";
    }
    function release()
    {
        state = "Normal"
    }

    gradient: Gradient {
        GradientStop {
            id: gradianstop1
            position: 0.060
            color: "#cacaca"
        }

        GradientStop {
            id: gradianstop2
            position: 0.950
            color: "#8b8b8b"
        }
    }
    states: [
        State {
            name: "Released"

            PropertyChanges {
                target: gradianstop1
                position: 0.060
                color: "#cacaca"
            }

            PropertyChanges {
                target: gradianstop2
                position: 0.950
                color: "#8b8b8b"
            }
            PropertyChanges {
                target: icon
                source: normalImage
            }
        },
        State {
            name: "Pushed"

            PropertyChanges {
                target: gradianstop1
                position: 0
                color: "#6b6b6b"
            }

            PropertyChanges {
                target: gradianstop2
                position: 0.410
                color: "#535353"
            }
            PropertyChanges {
                target: icon
                source: pushedImage
            }

        }
    ]
    MouseArea{
        anchors.fill: parent
        onPressed: {
            state = "Pushed"
            parent.onPressed()
        }
        onReleased: {
            state = "Normal"
            if(!latch)
                parent.onReleased()
        }
    }
    Image {
        id: icon
        x: 15
        y: 25
        width: 20
        height: 20
        source:  normalImage
    }

    border.color: "#000000"
}
