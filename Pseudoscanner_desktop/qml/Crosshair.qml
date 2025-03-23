import QtQuick 2.15

Item {
    id: root
    width: 48
    height: 48
    
    // Горизонтальная линия
    Rectangle {
        width: parent.width
        height: 1
        color: "#ff0000"
        anchors.verticalCenter: parent.verticalCenter
        antialiasing: true
    }

    // Вертикальная линия
    Rectangle {
        width: 1
        height: parent.height
        color: "#ff0000"
        anchors.horizontalCenter: parent.horizontalCenter
        antialiasing: true
    }

    // Центральная точка
    Rectangle {
        width: 3
        height: 3
        radius: 1.5
        color: "#ff0000"
        anchors.centerIn: parent
        antialiasing: true
    }
}