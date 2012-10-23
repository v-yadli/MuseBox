// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import DataModel 1.0
import TrackArrangement 1.0

Item {
    property NoteModel rowModel:null
    onRowModelChanged: {
        console.log("rowModel changed")
        console.log(rowModel)
        //view.model = rowModel
    }
    ListView{
        id:view
        orientation: ListView.Horizontal
        model: null
        delegate: Component{
            PatternNoteDisplay{
                ListView.onAdd:{
                    console.log("Adding pattern note")
                }
                id:display
                height:10
                width:10
                name: name
                token: token
                length: length
                offset: offset
                padding: padding
            }
        }
    }
}
