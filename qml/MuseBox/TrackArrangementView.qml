// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import TrackArrangement 1.0

Item {

    clip: false

    property int unitCountInBeat : 3
    property int beatCountInBar : 3
    property int barLength : 120
    property int beatLength : barLength / beatCountInBar
    width: 1000
    height: 500

    function resetInterface()
    {
        flick.contentX = 0;
    }

    function insertNote(draggedNote){
        arrangementView.droppedNote = draggedNote //miraculous!
        draggedNoteRecycleTimer.start()
    }

    function setCurrentPos(bar,beat,beatPos) {
        currentPos.x = bar*barLength + (beat+beatPos)*beatLength
    }

    function setLoopPos(lStart,lEnd)
    {
        loopStartPos.x = lStart
        loopEndPos.x = lEnd
    }
    function setContentY(y) {
        flick.contentY = y
    }

    Timer{
        id: draggedNoteRecycleTimer
        interval: 50
        repeat: false
        onTriggered: {
            if(arrangementView.droppedNote !== undefined){
                arrangementView.droppedNote.destroy()
                arrangementView.droppedNote = undefined
            }
        }
    }

    function setContentHeight(h)
    {
        arrangementView.height = h
    }

    function setBeatBarCount(beatInBar,unitInBeat)
    {
        ruler.beatCount = beatInBar;
        ruler.unitCount = unitInBeat;
    }


    TrackArrangementBackground{
        z:-1
        x:(-flick.contentX) % 120
        id:arrangementBackground
        height:parent.height
        width:parent.width*1.5
    }

    Flickable {
        flickableDirection: Flickable.HorizontalFlick
        boundsBehavior: Flickable.StopAtBounds
        z:0
        id: flick
        clip: false
        contentWidth: 10000


        anchors{
            top: parent.top
            topMargin: 10
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        y:0

        Item{
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
                            z:-1
                            id: dragDropArea
                            property bool clicked: false
                            property int startX:0
                            property int startY:0
                            hoverEnabled: true
                            anchors.fill: parent
                            onPositionChanged: {
                                if(arrangementView.droppedNote !== undefined){
                                    console.log("Collecting note");
                                    var note = arrangementView.droppedNote
                                    arrangementView.droppedNote = undefined
                                    if(note !== undefined)
                                    {
                                        //TODO the moved note has a moving flag. So it can be returned
                                        //Instead of destroyed
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
                        trackIndex: index
                        rowModel : arrangement
                        width: parent.width
                        height: 80
                    }
                }
            }
        }
    }


    Item{
        id: rulers
        x:(-flick.contentX) % 120
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        width: flick.width * 1.5

        CursorRuler{
            id:ruler
            height:10
            unitCount: 3
            beatCount: 3
            anchors.left:parent.left
            anchors.right:parent.right
            anchors.top:parent.top
        }
    }
    Item{//Layer II -- cursors
        z:1
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        anchors.left:parent.left
        anchors.right:parent.right

        Item{
            MouseArea{
                anchors{
                    top:parent.top
                    left:parent.left
                    right:parent.right
                }
                height:10
                property bool isClick : true

                onReleased:{
                    if(isClick){
                        console.log("ruler clicked!");
                        transposeMachine.setCurrentPosition(mouseX);
                    }
                }
                onPressed:{
                    isClick = true
                }
                onPositionChanged: {
                    isClick = false
                    transposeMachine.setCurrentPosition(mouseX);
                }
            }


            id: cursors
            x:(-flick.contentX)
            anchors.top:parent.top
            anchors.bottom:parent.bottom
            width:flick.contentWidth

            Rectangle{
                Rectangle{
                    x:0
                    y:0
                    height:5
                    width:5
                    color:"#0F0"
                }
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
                MouseArea{
                    x:0
                    y:0
                    height:10
                    width:10
                    onPositionChanged: {
                        transposeMachine.setLoopStart(mouseX + loopStartPos.x);
                    }
                }
                Rectangle{
                    x:0
                    y:0
                    height:5
                    width:5
                    color:"#F00"
                }

                width:1
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                }
                height:parent.height
                color: "#F00"
            }
            Rectangle{
                Rectangle{
                    x:0
                    y:0
                    height:5
                    width:5
                    color:"#00F"
                }
                MouseArea{
                    x:0
                    y:0
                    height:10
                    width:10
                    onPositionChanged: {
                        transposeMachine.setLoopEnd(mouseX + loopEndPos.x);
                    }
                }

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
