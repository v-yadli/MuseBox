// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "TransposeComponents"
import DataModel 1.0
import TrackArrangement 1.0

Rectangle {
    id: main
    width: 1024
    height: 768
    property string projectName : ""
    function resetInterface()
    {
        patternList.currentModel = undefined
        trackView.resetInterface()
    }
    function exportSong(){
        guiTimer.stop()
        musebox.exportSong()
        guiTimer.start()
    }
    function loadProject()
    {
        guiTimer.stop()
        projectName = musebox.loadProject("");
        resetInterface();
        guiTimer.start()
    }
    function saveProject()
    {
        guiTimer.stop()
        projectName = musebox.saveProject(projectName);
        guiTimer.start()
    }
    function newProject()
    {
        console.log("Entering new project")

        guiTimer.stop()

        musebox.newProject();
        resetInterface();

        guiTimer.start()
    }

    property int selectedTrack : -1

    function selectTrack(index, patternModel)
    {
        console.log("track "+index+" selected");
        selectedTrack = index
        patternList.currentModel = patternModel
    }

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
        property bool entered : false
        onTriggered: {
            if(entered)return;
            entered = true
            var bar = transposeMachine.getBar()
            var beat = transposeMachine.getBeat()
            var min = transposeMachine.getMinute()
            var sec = transposeMachine.getSecond()
            var mil = transposeMachine.getMillisecond()
            var beatPos = transposeMachine.getPositionInBeat()
            var lStart = transposeMachine.getLoopStart()
            var lEnd = transposeMachine.getLoopEnd()

            trackView.checkTrackCount()//Weird...

            transpose.updateTimeAndBeat(min,sec,mil,bar,beat)
            trackView.setCurrentPos(bar,beat,beatPos)
            trackView.setLoopPos(lStart,lEnd);

            transpose.updateMasterDbMeter(musebox.masterL(), musebox.masterR())
            for(var i=0;i<trackView.trackCount();++i)
            {
                trackView.updateDbMeter(i,musebox.query_dBL(i),musebox.query_dBR(i))
            }
            entered = false
        }
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

        front: PatternList{
            id:patternList
            currentModel:undefined
        }
    }
}
