// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: 500
    height: 500
    color: "#00000000"
    clip: false

    property int unitCountInBeat : 3
    property int beatCountInBar : 3
    property int barLength : 60
    property int beatLength : barLength / unitCountInBeat

    function setCurrentPos(bar,beat,beatPos)
    {
        trackArrangementFlick.setCurrentPos(bar,beat,beatPos)
    }

    Flickable {
        id: trackViewVerticalFlick
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        clip: true

        onWidthChanged:{
            trackViewRow.width = width
        }
        onHeightChanged:{
            currentPos.height = height
            loopStartPos.height = height
            loopEndPos.height = height
        }

        Row {
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
                width: 200
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
            }

            Flickable {
                id: trackArrangementFlick
                flickableDirection: Flickable.HorizontalFlick
                width: 300
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0

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
                }
                Rectangle{
                    x:0
                    id: loopStartPos
                    width:1
                    height:500
                    color: "#F00"
                    z:10001
                }
                Rectangle{
                    x: barLength
                    id: loopEndPos
                    width:1
                    height:500
                    color: "#00F"
                    z:10002
                }
                Column {
                    id: trackArrangementContainer
                    x: 0
                    y: 0
                    width: 224
                    height: 358

                    Rectangle
                    {
                        width:100
                        height:100
                        color:"#0F0"
                    }
                    Rectangle
                    {
                        width:100
                        height:100
                        color:"#F00"
                    }
                }
            }
        }
    }

}
