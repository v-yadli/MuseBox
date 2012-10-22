// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 500
    height: 500
    color: "#FF000000"
    opacity: 1
    clip: false

    property int unitCountInBeat : 3
    property int beatCountInBar : 3
    property int barLength : 120
    property int beatLength : barLength / unitCountInBeat
    function setCurrentPos(bar,beat,beatPos)
    {
        hFlick.setCurrentPos(bar,beat,beatPos)
    }

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
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        clip: true

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
//                headerColumn.height = height
                headerView.height=height
                hFlick.height = height
                vFlick.contentHeight = height
            }
            ListView {
                id: headerView
                interactive:false
                x:0
                width:200
                height:0
                onHeightChanged: {
                    currentPos.height = height
                    loopStartPos.height = height
                    loopEndPos.height = height
                    headerView.height = height
                }

                model: trackModel
                delegate: Component {
                    TrackHeader{
                        trackName: name
                        onSelected:{
                            console.log(index)
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

                y:0
                height:parent.height

                onFlickEnded: {
                    console.log("horizon flick end")
                }

                onHeightChanged: {
                    arrangementView.height = height
                }

                clip: true
                function setCurrentPos(bar,beat,beatPos)
                {
                    currentPos.x = bar*barLength + (beat+beatPos)*beatLength
                }
                Rectangle{
                    x:0
                    id: currentPos
                    width:1
                    //anchors.top: parent <- doesn't work !!!
                    //anchors.bottom: parent
                    color: "#0F0"
                    z:10000
                    height:parent.height
                }
                Rectangle{
                    x:0
                    id: loopStartPos
                    width:1
                    height:parent.height
                    color: "#F00"
                    z:10001
                }
                Rectangle{
                    x: barLength
                    id: loopEndPos
                    width:1
                    height:parent.height
                    color: "#00F"
                    z:10002
                }
                ListView{
                    y: 0
                    property int rightMost: 1000
                    interactive: false
                    id: arrangementView
                    height:parent.height
                    width: rightMost//TODO : expand flick area when cannot hold new elements
                    model: trackModel
                    delegate : Component {
                        TrackArrangement{
                            width: parent.width
                        }
                    }
                }
            }
        }
    }
}
