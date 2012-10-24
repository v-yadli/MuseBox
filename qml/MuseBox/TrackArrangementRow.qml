// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import DataModel 1.0
import TrackArrangement 1.0

Item {
    property NoteModel rowModel:null
    property int trackIndex : -1
    id:base
    onRowModelChanged: {
        view.model = rowModel
    }
    Component{
        id: draggedNoteGenerator
        PatternNoteDisplay{
            property int targetTrack : -1
            property bool isMove:true
        }
    }

    Rectangle{
        height:1
        width:parent.width
        x:0
        y:0
    }

    Rectangle{
        height:1
        width:parent.width
        x:0
        y:parent.height-1
    }

    ListView{
        interactive: false
        anchors.fill: parent
        id:view
        orientation: ListView.Horizontal
        model: null
        delegate: Component{
            PatternNoteDisplay{
                MouseArea{
                    property variant draggedNote
                    property int startX
                    property int startY

                    anchors.fill:parent
                    preventStealing: true
                    onPressed:{
                        draggedNote = draggedNoteGenerator.createObject(parent,
                                                                        {
                                                                            "x":mouseX,
                                                                            "y":mouseY,
                                                                            "z":5000,
                                                                            "height":80,
                                                                            "token":notepattern,
                                                                            "offset":parent.offset,
                                                                            "length":parent.length,
                                                                            "name":parent.name,
                                                                            "padding":0,
                                                                            "targetTrack": trackIndex
                                                                        });
                        draggedNote.x -= draggedNote.width / 2;
                        draggedNote.y -= draggedNote.height / 2;
                        startX = mouseX
                        startY = mouseY
                        parent.height = 0
                    }
                    onPositionChanged:{
                        draggedNote.x += mouseX - startX
                        draggedNote.y += mouseY - startY
                        startX = mouseX
                        startY = mouseY
                    }
                    onReleased:{
                        trackView.insertNote(draggedNote);
                        rowModel.removeNote(parent.token,parent.notePosition);
                        //draggedNote.destroy() the track will destroy this
                    }
                }

                id:display
                height:80
                name: notename
                token: notepattern
                length: notelength
                offset: noteoffset
                padding: notepadding
                notePosition: noteposition
            }
        }
    }
}
