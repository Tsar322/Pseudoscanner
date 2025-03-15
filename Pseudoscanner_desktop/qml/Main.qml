import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Dialogs

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Button {
        id: button
        x: 0
        y: 19
        text: qsTr("Open files")
        onClicked: fileDialog.open()
    }

    FileDialog{
        id: fileDialog
        title: "Please choose a file"
        currentFolder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        nameFilters: ["Image files (*.jpg, *.png)"]
        fileMode: FileDialog.OpenFiles
        onAccepted: {
            app.openImages(selectedFiles)
        }
        onRejected: {
            console.log("File selection canceled")
        }
    }

    Image {
        id: originalImage
        x: 63
        y: 0
        width: 219
        height: 265
        //source: "image://colors/yellow"
        fillMode: Image.PreserveAspectFit
    }

    Image {
        id: processedImage
        x: 280
        y: 0
        width: 219
        height: 265
        //source: "image://colors/yellow"
        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: filePath
        x: 162
        y: 346
        width: 350
        height: 71
        text: qsTr(app.path(0))
        font.pixelSize: 12
    }

    Connections {
        target: app
        onCursorDataChanged: {
            filePath.text = qsTr(app.path(app.cursor))
            originalImage.source = "image://images/" + app.cursor + "/original"
            //processedImage.source = "image://images/" + app.cursor + "/processed"
            processedImage.source = "image://colors/yellow"
        }
    }

}
