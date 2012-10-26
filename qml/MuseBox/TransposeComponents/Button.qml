// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 50
    height: 70
    radius: 0
    signal pressed
    signal released
    signal pushed
    property bool preventTriggerIfOutOfBound : true
    property string normalImage : "Stop.png"
    property string pushedImage : "StopHL.png"
    property bool latch : false
    function isPushed(){
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
            if(!latch){
                parent.state = "Pushed"
                parent.pressed()
            }else{
                if(parent.state == "Pushed")
                    parent.state = "Normal"
                else
                    parent.state = "Pushed"
            }
        }
        onReleased: {
            if(!latch) {
                parent.state = "Normal"
                if((mouseX >= 0 && mouseY >= 0 && mouseX < width && mouseY < height) || !preventTriggerIfOutOfBound)
                    parent.released()
            }else{
                if(parent.state == "Normal")
                    parent.released()
                else
                    parent.pushed()
            }
        }
    }
    Image {
        id: icon
        anchors.centerIn: parent
        width: 20
        height: 20
        source:  normalImage
    }

    border.color: "#000000"
}
