// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import TrackArrangement 1.0

Image {

    source:"navy_blue.png"
    fillMode: Image.Tile

    clip: false

    property int unitCountInBeat : 3
    property int beatCountInBar : 3
    property int barLength : 120
    property int beatLength : barLength / unitCountInBeat

    function addTrack()
    {
        if((headerView.count + 1) * 80 >= row.height)
        {
            row.height = Math.max( (headerView.count + 1) * 80, height)
        }
        trackModel.addTrack()
    }

    function trackCount()
    {
        return headerView.count;
    }

    function updateDbMeter(track,l,r)
    {
        headerView.currentIndex=track
        headerView.currentItem.updateDbMeter(l,r)
    }
    function setCurrentPos(bar,beat,beatPos)
    {
        currentPos.x = bar*barLength + (beat+beatPos)*beatLength
    }

    onHeightChanged:{
        cursors.height = height
    }

/*****
  Layout map:
  [Layer 1] y-offset=10
  vFlick[Full viewport] -> Row
    Row width controlled by vFlick width
    vFlick content height controlled by row height
  Row-> [ HeaderView, hFlick ]
    Row height controlled by max[track count*track height, vFlickHeight]
    HeaderView height linked to row
    HeaderView width fixed to 200
    hFlick height linked to row
    hFlick width <- row.width - HeaderView.width

    hFlick->arrangementView
      arrangementView height controlled by hFlick
  [Layer2] y-offset=0

*/
    Flickable {//The outside vertical flickable has a same size of the viewport
        boundsBehavior: Flickable.StopAtBounds
        id: vFlick
        anchors{
            top:parent.top
            topMargin: 10
            bottom:parent.bottom
            left:parent.left
            right:parent.right
        }
        flickableDirection: Flickable.VerticalFlick
        clip: true
        contentHeight:row.height

        onWidthChanged:{
            row.width=width
        }
        onHeightChanged:{
            row.height= Math.max((headerView.count*80),height)
        }

        Row {
             //This row splits horizontal space to header view and arrangement view.
             //The height of the row is linked to the height of the viewport, but it's not important
             //The width of the row decides the width of the trackArrangementView
            id: row
            x:0
            y:0
            height:0
            onWidthChanged: {
                hFlick.width = width - headerView.width
            }
            onHeightChanged: {
                //headerView.height=height
                //hFlick.height = height
                //vFlick.contentHeight = height
            }
            ListView {
                id: headerView
                interactive:false
                x:0
                width:200
                height:parent.height
                onHeightChanged: {
                }

                model: trackModel
                delegate: Component {
                    TrackHeader{
                        trackName: name
                        recordingFlag: recording
                        onSelected:{
                            main.selectTrack(index,patterns)
                            console.log(index)
                        }
                        onRecordingSettingChanged: {
                            trackModel.setData(index,"recording",flag)
                        }
                        onSetTrackName:
                        {
                            trackModel.setData(index,"name",newText)
                        }
                    }
                }
            }

            Flickable {
                id: hFlick
                boundsBehavior: Flickable.StopAtBounds
                flickableDirection: Flickable.HorizontalFlick
                width:row.width-headerView.width
                height:row.height
                contentWidth: Math.max(width + 120*3,arrangementView.rightMost)
                y:0

                onHeightChanged: {
                    arrangementView.height = height
                }

                clip: true
                TrackArrangementBackground{
                    id:arrangementBackground
                    height:parent.height
                    width:parent.width + 120*3
                    z:0
                }
                ListView{
                    y: 0
                    property int rightMost: 1000
                    interactive: false
                    id: arrangementView
                    height:parent.height
                    width: rightMost//TODO : expand flick area when cannot hold new elements
                    model: null
                    delegate : Component {
                        TrackArrangementRow{
                            ListView.onAdd: {
                                console.log("Adding new TrackArrangementRow");
                            }
                            id: arrangementRow
                            rowModel : arrangement
                            width: parent.width
                            height: 80
                        }
                    }
                    z:1
                }

            }
        }
    }

    Item{//Layer II -- cursors
        x:200
        anchors.top:parent.top
        anchors.bottom:parent.bottom
        anchors.left:parent.left
        anchors.leftMargin: headerView.width
        anchors.right:parent.right
        clip: true

        Item{
            id: cursors
            x:-hFlick.contentX
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
