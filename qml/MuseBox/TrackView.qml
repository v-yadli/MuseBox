import QtQuick 1.1

Item{
    anchors.fill:parent
    function trackCount()
    {
        return headerView.trackCount();
    }
    function addTrack()
    {
        var h =  Math.max((headerView.trackCount()+1)*80, parent.height);
        headerView.setContentHeight(h)
        arrangementView.setContentHeight(h)
        trackModel.addTrack()
        //console.log(headerView.trackCount())
    }
    function updateDbMeter(track,l,r)
    {
        headerView.updateDbMeter(track,l,r)
    }
    function insertNote(draggedNote){
        arrangementView.insertNote(draggedNote)
    }
    function setCurrentPos(bar,beat,beatPos)
    {
        arrangementView.setCurrentPos(bar,beat,beatPos)
    }

    TrackArrangementView{
        id:arrangementView
        x:200
        anchors{
            left:headerView.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }
    Rectangle{
        x:0
        width:200
        height:10
        y:0
        color:"#313131"
    }
    TrackHeaderView{
        id:headerView
        x:0
        width:200
        height:parent.height
        onContentYChanged: {
            arrangementView.setContentY(contentY)
        }
        anchors{
            top: parent.top
            bottom: parent.bottom
            left:parent.left
        }
    }
}
