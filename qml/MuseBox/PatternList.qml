// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import TrackArrangement 1.0

Rectangle{
    anchors.fill: parent
    property variant currentModel : undefined
    width: 100
    height: 500
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#474747"
        }

        GradientStop {
            position: 0.950
            color: "#1c1c1c"
        }
    }
    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.MidButton
        property int startY
        onPressed: {
            startY = mouseY
            console.log("startY = ",startY);
        }
        onPositionChanged: {
            patternList.contentY += (startY - mouseY)
            if(patternList.contentY < 0)
                patternList.contentY = 0
            if(patternList.contentY > patternList.contentHeight)
                patternList.contentY = patternList.contentHeight;
            startY = mouseY
        }
    }
    ListView{
        id: patternList
        interactive:true//TODO make a slider for it
        anchors.fill:parent
        model:currentModel
        delegate:Component{
            PatternDisplay{
                id: patternDisplay
                property variant draggedNote
                property int startX
                property int startY
                MouseArea{
                    anchors.fill:parent
                    preventStealing: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onPressed:{
                        draggedNote = draggedNoteGenerator.createObject(parent,
                                                                        {
                                                                            "x":mouseX,
                                                                            "y":mouseY,
                                                                            "z":5000,
                                                                            "height":80,
                                                                            "token":parent.token,
                                                                            "offset":0,
                                                                            "length":parent.length(),
                                                                            "name":"",
                                                                            "padding":0,
                                                                            "targetTrack": selectedTrack
                                                                        });
                        draggedNote.x -= draggedNote.width / 2;
                        draggedNote.y -= draggedNote.height / 2;
                        startX = mouseX
                        startY = mouseY
                    }
                    onPositionChanged:{
                        draggedNote.x += mouseX - startX
                        draggedNote.y += mouseY - startY
                        startX = mouseX
                        startY = mouseY
                    }
                    onReleased:{
                        trackView.insertNote(draggedNote);
                        //draggedNote.destroy() the track will destroy this
                    }
                }
                token: pattern
                height:50
                width:100
                //TODO name
            }
        }
    }
    Rectangle {
        id: scrollbar
        anchors.right: patternList.right
        y: patternList.visibleArea.yPosition * patternList.height
        width: 4
        height: patternList.visibleArea.heightRatio * patternList.height
        color: "white"
        opacity:0.4
    }
}
