// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import TrackArrangement 1.0

Item {

    clip: false

    property int unitCountInBeat : 3
    property int beatCountInBar : 3
    property int barLength : 120
    property int beatLength : barLength / unitCountInBeat
    width: 1000
    height: 500


    function insertNote(draggedNote){
        arrangementView.droppedNote = draggedNote //miraculous!
    }

    function setCurrentPos(bar,beat,beatPos)
    {
        currentPos.x = bar*barLength + (beat+beatPos)*beatLength
    }

    function setContentY(y)
    {
        flick.contentY = y
    }

    function setContentHeight(h)
    {
        flick.contentHeight = h
        arrangementView.height = h
        console.log("setContentHeight : "+h);
    }
    TrackArrangementBackground{
        z:-1
        x:-flick.contentX
        id:arrangementBackground
        height:parent.height
        width:parent.width + 120*3
    }

    Flickable {
        z:0
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.HorizontalFlick
        id: flick
        clip: false
        contentHeight:parent.height
        contentWidth: Math.max(width + 120*3,arrangementView.rightMost)

        anchors{
            top: parent.top
            topMargin: 10
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        y:0

        ListView{
            y: 0
            property int rightMost: 1000
            property variant droppedNote : undefined
            interactive: false
            id: arrangementView
            height:parent.height
            width: rightMost//TODO : expand flick area when cannot hold new elements
            model: trackModel
            z:1
            delegate : Component {
                TrackArrangementRow{
                    MouseArea{
                        id: dropDetectionArea
                        hoverEnabled: true
                        anchors.fill: parent
                        onPositionChanged: {
                            if(arrangementView.droppedNote !== undefined){
                                var note = arrangementView.droppedNote
                                arrangementView.droppedNote = undefined
                                if(note !== undefined)
                                {
                                    if(note.targetTrack !== index){
                                        console.log("posted to wrong track. destroying.");
                                        note.destroy()
                                    }else{
                                        console.log("Incoming note!!");
                                        //TODO Snap to grid/free, currently only snapping to bar..
                                        var bar = Math.floor(mouseX / 120)
                                        arrangement.insertNote(note.token,bar,0);
                                        note.destroy()
                                    }
                                }
                            }
                        }
                    }
                    id: arrangementRow
                    rowModel : arrangement
                    width: parent.width
                    height: 80
                }
            }
        }
    }


    Item{//Layer II -- cursors
        z:1
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        anchors.left:parent.left
        anchors.right:parent.right

        Item{
            id: cursors
            x:-flick.contentX
            anchors.top:parent.top
            anchors.bottom:parent.bottom
            width: parent.width + 120*3

            CursorRuler{
                anchors.top:parent.top
                height:10
                unitCount: 3
                beatCount: 3
                anchors.left:parent.left
                anchors.right:parent.right
            }

            Rectangle{
                x:0
                id: currentPos
                width:1
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                }
                color: "#0F0"
                height:parent.height
            }
            Rectangle{
                x:0
                id: loopStartPos
                width:1
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                }
                height:parent.height
                color: "#F00"
            }
            Rectangle{
                x: barLength
                id: loopEndPos
                width:1
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                }
                height:parent.height
                color: "#00F"
            }
        }
    }
}
