// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id: base
    width: 150
    height: 20

    Image {
        id: bg
        anchors.fill: parent
        source: "HorizonDBMeterBG.png"
    }

    Image {
        id: lChannel
        x: 0
        y: 0
        width: 150
        height: 8
        sourceSize.width: 150
        fillMode: Image.Tile
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        source: "HorizonDBMeterFG.png"
    }

    Image {
        id: rChannel
        x: 0
        y: 12
        width: 150
        height: 8
        fillMode: Image.Tile
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        source: "HorizonDBMeterFG.png"
    }

    function update(ldB,rdB)// range from -100 to 0
    {
        ldB = 20 * Math.log(ldB) / Math.LOG10E
        rdB = 20 * Math.log(rdB) / Math.LOG10E

        if(ldB < -100)
            ldB = -100
        if(rdB < -100)
            rdB = -100
        if(ldB > 0)
            ldB = 0
        if(rdB > 0)
            rdB = 0

        lChannel.width = width * ((100+ldB) / 100)
        rChannel.width = width * ((100+rdB) / 100)
    }

}
