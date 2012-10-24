// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "TransposeComponents"
import DataModel 1.0
import TrackArrangement 1.0

Rectangle {
    id: main
    width: 1024
    height: 768
    Transpose{
        id:transpose
        y: 670
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        z: 2
    }

    Timer{
        id:guiTimer
        interval: 30
        running:true
        repeat: true
        onTriggered: {
            var bar = transposeMachine.getBar()
            var beat = transposeMachine.getBeat()
            var min = transposeMachine.getMinute()
            var sec = transposeMachine.getSecond()
            var mil = transposeMachine.getMillisecond()
            var beatPos = transposeMachine.getPositionInBeat()
            var lStart = transposeMachine.getLoopStart()
            var lEnd = transposeMachine.getLoopEnd()

            transpose.updateTimeAndBeat(min,sec,mil,bar,beat)
            trackView.setCurrentPos(bar,beat,beatPos)
            trackView.setLoopPos(lStart,lEnd);

            transpose.updateMasterDbMeter(musebox.masterL(), musebox.masterR())
            for(var i=0;i<trackView.trackCount();++i)
            {
                trackView.updateDbMeter(i,musebox.query_dBL(i),musebox.query_dBR(i))
            }
        }
    }

    property int selectedTrack : -1

    function selectTrack(index, patternModel)
    {
        console.log("track "+index+" selected");
        selectedTrack = index
        patternList.model = patternModel
    }

    Flipable {
        function updateTrackViewCurrentPosition(bar,beat,beatPos)
        {
            trackView.setCurrentPos(bar,beat,beatPos)
        }

        id: trackFlip//Track flip contains track view and mixer
        y: 0
        anchors.right: patternFlip.left
        anchors.rightMargin: 0
        anchors.bottom: transpose.top
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        front:  TrackView {
            id: trackView
        }
    }

    Flipable {
        id: patternFlip
        width:100
        anchors.bottom: transpose.top
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        Component{
            id: draggedNoteGenerator
            PatternNoteDisplay{
                property int targetTrack : -1
            }
        }

        front:ListView{
            id:patternList
            interactive:false//TODO make a slider for it
            anchors.fill:parent
            model:null
            delegate:Component{
                PatternDisplay{
                    id: patternDisplay
                    property variant draggedNote
                    property int startX
                    property int startY
                    MouseArea{
                        anchors.fill:parent
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
    }
}
