import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Dialogs
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    RowLayout{
        anchors.fill: parent
        Item{
            implicitWidth: 140
            Layout.fillHeight: true
            ColumnLayout{
                width: 140
                height: parent.height
                Button {
                    id: button
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    text: qsTr("Open files")
                    onClicked: fileDialog.open()
                }

                ListView{
                    id: pagesListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    orientation: ListView.Vertical
                    rightMargin: verticalScrollBar.width

                    model: app
                    cacheBuffer: 2000
                    clip: true

                    property int selectedIndex: -1

                    delegate: Rectangle{
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 180
                        width: 100
                        color: index === pagesListView.selectedIndex ? "lightblue" : "transparent"

                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                pagesListView.selectedIndex = index
                                app.setCursor(index)
                            }
                        }

                        Column{
                            Image {
                                width: 100
                                height: 150
                                source: model.originalImage
                                fillMode: Image.PreserveAspectFit
                                cache: true
                            }
                            Text{
                                width: 100
                                //height: 30
                                text: model.name
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }
                            Text{
                                text: "Placeholder" + index
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar{
                        id: verticalScrollBar
                    }
                }
            }
        }
        ColumnLayout{
            Layout.fillHeight: true
            Layout.fillWidth: true
            RowLayout{
                Layout.fillHeight: true
                Layout.fillWidth: true
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Image {
                        id: originalImage
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit

                        property size originalSize: Qt.size(implicitWidth, implicitHeight)
                        property real scaleFacor: Math.min(paintedWidth / implicitWidth, paintedHeight / implicitHeight)

                        // Границы отрисованного изображения
                        property real imageX: (parent.width - paintedWidth) / 2
                        property real imageY: (parent.height - paintedHeight) / 2
                        property real imageWidth: paintedWidth
                        property real imageHeight: paintedHeight

                        function onGeometryChanged(){
                            console.log("in onGeometryChanged")
                            imageX = (parent.width - paintedWidth) / 2
                            imageY = (parent.height - paintedHeight) / 2
                            imageWidth = paintedWidth
                            imageHeight = paintedHeight
                            scaleFacor = Math.min(paintedWidth / implicitWidth, paintedHeight / implicitHeight)

                            if (status === Image.Ready){
                                pointsRepeater.rewritePoints()
                            }
                        }

                        onStatusChanged: if (status === Image.Ready){
                            pointsRepeater.rewritePoints()
                        }
                        onXChanged: onGeometryChanged()
                        onYChanged: onGeometryChanged()
                        onWidthChanged: onGeometryChanged()
                        onHeightChanged: onGeometryChanged()
                        onPaintedGeometryChanged: onGeometryChanged()
                        onSourceChanged: {
                            originalSize = Qt.size(implicitWidth, implicitHeight)
                            scaleFacor =Math.min(paintedWidth / implicitWidth, paintedHeight / implicitHeight)
                        }

                        Repeater {
                            id: pointsRepeater
                            model: 4
                            delegate: DraggablePoint {

                                // Важно: Используем imageHeight из изображения
                                minX: originalImage.imageX
                                maxX: originalImage.imageX + originalImage.imageWidth
                                minY: originalImage.imageY
                                maxY: originalImage.imageY + originalImage.imageHeight


                                Component.onCompleted: initPosition()

                                function recalcConstrains(){
                                    minX = originalImage.imageX
                                    maxX = originalImage.imageX + originalImage.imageWidth
                                    minY = originalImage.imageY
                                    maxY = originalImage.imageY + originalImage.imageHeight
                                }

                                function initPosition() {
                                    //originalImage.onGeometryChanged()

                                    minX = originalImage.imageX
                                    maxX = originalImage.imageX + originalImage.imageWidth
                                    minY = originalImage.imageY
                                    maxY = originalImage.imageY + originalImage.imageHeight

                                    console.log("in initPosition()");
                                    var originalX = app.getCorner(app.cursor, index, true);
                                    var originalY = app.getCorner(app.cursor, index, false);
                                    var paintedX = originalX * originalImage.scaleFacor;
                                    var paintedY = originalY * originalImage.scaleFacor;

                                    x = minX + paintedX
                                    y = minY + paintedY
                                }

                                onDragged:{
                                    console.log("Dragged");
                                    var paintedX = x - minX
                                    var paintedY = y - minY

                                    var originalX = paintedX / originalImage.scaleFacor
                                    var originalY = paintedY / originalImage.scaleFacor

                                    app.setCorner(app.cursor, index, originalX, originalY)
                                }
                            }

                            function rewritePoints() {
                                for(var i = 0; i < pointsRepeater.count; i++){
                                    var point = pointsRepeater.itemAt(i)
                                    if(point){
                                        point.initPosition()
                                    }
                                }
                            }
                        }
                    }


                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Image {
                        id: processedImage
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
            Text {
                id: filePath
                Layout.fillWidth: true
                height: 100
                text: qsTr(app.path(0))
                font.pixelSize: 12
            }
            RowLayout{
                height: 150
                Layout.fillWidth: true

                Button{
                    id: predetectCornersButton
                    Layout.fillWidth: true
                    text: "Predetect Corners"

                    onClicked: {
                        app.predetectCorners(app.cursor)
                        pointsRepeater.rewritePoints()
                    }
                }
                Button{
                    id: transformButton
                    Layout.fillWidth: true
                    text: "Transform"

                    onClicked: {
                        app.applyTransform(app.cursor)
                    }
                }
                Button{
                    id: saveImageButton
                    Layout.fillWidth: true
                    text: "saveImage"

                    onClicked: {
                        saveImageDialog.open()
                    }
                }
            }
        }
    }

    FileDialog{
        id: fileDialog
        title: "Please choose a file"
        currentFolder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        nameFilters: ["Image files (*jpeg *jpg *png)"]
        fileMode: FileDialog.OpenFiles
        onAccepted: {
            app.openImages(selectedFiles)
        }
        onRejected: {
            console.log("File selection canceled")
        }
    }

    FileDialog{
        id: saveImageDialog
        fileMode: FileDialog.SaveFile
        nameFilters: ["PNG Image (*.png)", "JPEG Image (*.jpg *.jpeg)", "All files (*)"]
        defaultSuffix: "png"
        onAccepted: {
            app.saveImage(app.index, selectedFile)
        }
    }

    Connections {
        target: app
        onCursorDataChanged: {
            filePath.text = qsTr(app.path(app.cursor))
            originalImage.source = "image://images/" + app.cursor + "/original"
            processedImage.source = "image://images/" + app.cursor + "/processed"
        }
        onProcessedImageChanged: {
            console.log("in onProcessedImageChanged");
            processedImage.source = "image://garbage"
            processedImage.source = "image://images/" + app.cursor + "/processed"
        }
        onCornersPredetected:{
            if(app.cursor === index){
                pointsRepeater.rewritePoints()
            }
        }
    }

}
