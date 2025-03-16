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
                    spacing: 10
                    model: app
                    cacheBuffer: 2000
                    clip: true

                    delegate: Column{
                        topPadding: 10
                        rightPadding: verticalScrollBar.width
                        spacing: 5
                        anchors.horizontalCenter: parent.horizontalCenter
                        Image {
                            height: 150
                            width: 100
                            source: model.originalImage
                            fillMode: Image.PreserveAspectFit
                            cache: true
                        }
                        Text{
                            width: 100
                            height: 30
                            text: model.name
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }
                    }

                    ScrollBar.vertical: ScrollBar{
                        id: verticalScrollBar
                        //width: 10
                        //policy: ScrollBar.AlwaysOn
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
                Image {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    id: originalImage
                    fillMode: Image.PreserveAspectFit
                }

                Image {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    id: processedImage
                    fillMode: Image.PreserveAspectFit
                }
            }
            Text {
                id: filePath
                Layout.fillWidth: true
                height: 100
                text: qsTr(app.path(0))
                font.pixelSize: 12
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

    Connections {
        target: app
        onCursorDataChanged: {
            filePath.text = qsTr(app.path(app.cursor))
            originalImage.source = "image://images/" + app.cursor + "/original"
            processedImage.source = "image://images/" + app.cursor + "/processed"
        }
    }

}
