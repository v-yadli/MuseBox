// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 500
    height: 500
    color: "#00000000"
    opacity: 1
    clip: false

    property int unitCountInBeat : 3
    property int beatCountInBar : 3
    property int barLength : 120
    property int beatLength : barLength / unitCountInBeat

    function setCurrentPos(bar,beat,beatPos)
    {
        trackArrangementFlick.setCurrentPos(bar,beat,beatPos)
    }

    function addTrack()
    {
        if((trackHeaderView.count + 1) * 80 >= trackHeaderContainer.height)
        {
            trackHeaderContainer.height += 80
            trackArrangementContainer.height += 80
        }
        trackModel.addTrack()
    }

    function trackCount()
    {
        return trackHeaderView.count;
    }

    function updateDbMeter(track,l,r)
    {
        trackHeaderView.currentIndex=track
        trackHeaderView.currentItem.updateDbMeter(l,r)
    }

    Flickable {
        boundsBehavior: Flickable.StopAtBounds
        id: trackViewVerticalFlick
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        clip: true

        onWidthChanged:{
            trackViewRow.width = width
        }
        onHeightChanged:{

            trackViewRow.height = height
        }

        Row {//The height of the row is not important
            id: trackViewRow
            width: 500
            height: 552
            x:0
            y:0
            onWidthChanged: {
                trackArrangementFlick.width = width - trackHeaderContainer.width
            }

            Column {
                id: trackHeaderContainer
                height:parent.height
                width:200

                onHeightChanged: {
                    currentPos.height = height
                    loopStartPos.height = height
                    loopEndPos.height = height
                    trackHeaderView.height = height
                    trackArrangementFlick.height = height

                    //also adjust the vertical flick's content height
                    //Don't move this line to Row layout, it will cause startup freezing
                    trackViewVerticalFlick.contentHeight = height

                }

                ListView {
                    id: trackHeaderView
                    height:parent.height
                    model: trackModel
                    delegate: Component {
                        TrackHeader{
                            trackName: name
                        }
                    }
                }
            }

            Flickable {
                boundsBehavior: Flickable.StopAtBounds
                id: trackArrangementFlick
                flickableDirection: Flickable.HorizontalFlick

                y:0
                x:200


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
                Column {
                    property int rightMost: 1000
                    id: trackArrangementContainer
                    x: 0
                    y: 0
                    width: rightMost//TODO : expand flick area when cannot hold new elements
                    height: parent.height
                }
            }
        }
    }

}
