// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle{
    width:200
    height: 1000
    gradient: Gradient {
        GradientStop {
            position: 0.280
            color: "#313131"
        }

        GradientStop {
            position: 0.930
            color: "#000000"
        }
    }
    anchors{
        top:parent.top
        topMargin:10
        bottom:parent.bottom
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
    function setContentHeight(h)
    {
        headerView.height = h
        flick.contentHeight = h
    }
    property int contentY:flick.contentY
    Flickable{
        id: flick
        width:parent.width
        height:parent.height
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ListView {
            id: headerView
            interactive: false
            x:0
            height:parent.height
            width:200
            onHeightChanged: {
                console.log(height)
            }
            model: trackModel
            delegate: Component {
                TrackHeader{
                    trackName: name
                    recordingFlag: recording
                    ListView.onAdd: {
                    }
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
    }
}
