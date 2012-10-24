// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import DataModel 1.0
import TrackArrangement 1.0

Item {
    property NoteModel rowModel:null
    onRowModelChanged: {
        view.model = rowModel
    }
    ListView{
        anchors.fill: parent
        id:view
        orientation: ListView.Horizontal
        model: null
        delegate: Component{
            PatternNoteDisplay{
                id:display
                height:80
                name: notename
                token: notepattern
                length: notelength
                offset: noteoffset
                padding: notepadding
            }
        }
    }
}
