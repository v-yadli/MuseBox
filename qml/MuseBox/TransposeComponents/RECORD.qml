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
                source: "Record.png"
            }
        },
        State {
            name: "Pushed"

            PropertyChanges {
                target: gradianstop1
                position: 0.040
                color: "#ff5555"
            }

            PropertyChanges {
                target: gradianstop2
                position: 0.530
                color: "#b13838"
            }

            PropertyChanges {
                target: image1
                source: "RecordHL.png"
            }
        }

    ]
    clip: false
    border.color: "#000000"

    MouseArea{
        anchors.fill: parent
        onClicked: {
            if(parent.state == "Pushed") {
                parent.state = "Released"
                musebox.setRecord(false)
            }else{
                parent.state = "Pushed"
                musebox.setRecord(true)
            }
        }
    }

    Image {
        id: image1
        x: 13
        y: 23
        width: 25
        height: 25
        source: "Record.png"
    }
}
