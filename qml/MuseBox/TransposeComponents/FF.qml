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

            PropertyChanges {
                target: image1
                source: "FF.png"
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
                source: "FFHL.png"
            }
        }

    ]
    border.color: "#000000"

    MouseArea{
        anchors.fill: parent
        onPressed: {
            parent.state= "Pushed"
            musebox.moveStart()
            musebox.setSpeed(2.0)
        }
        onReleased: {
            parent.state= "Released"
            musebox.moveEnd()
            musebox.setSpeed(1.0)
        }
    }
    Image {
        id: image1
        x: 10
        y: 20
        width: 30
        height: 30
        source: "FF.png"
    }
}
