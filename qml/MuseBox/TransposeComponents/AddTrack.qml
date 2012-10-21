// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 50
    height: 70
    radius: 0
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
                target: image1
                source: "AddTrackHL.png"
            }

        }
    ]
    MouseArea{
        anchors.fill: parent
        onPressed: {
            parent.state= "Pushed"
            trackModel.addTrack()
        }
        onReleased: {
            parent.state= "Released"
        }
    }
    Image {
        id: image1
        x: 15
        y: 25
        width: 20
        height: 20
        source: "AddTrack.png"
    }

    clip: true
    border.color: "#000000"
}
