// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import TrackArrangement 1.0

ListView{
    interactive:true//TODO make a slider for it
    anchors.fill:parent
    property variant currentModel : undefined
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
