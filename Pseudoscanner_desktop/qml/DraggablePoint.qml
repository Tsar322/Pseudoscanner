import QtQuick 2.15

Rectangle {
    id: root
    width: 0
    height: 0
    color: "transparent"
	
	signal dragged()
	
	property real imageBasedX
	property real imageBasedY
    
    // Явные объявления свойств (как в оригинале)
    property real minX
    property real maxX
    property real minY
    property real maxY

    // Перекрестие центрировано относительно этого элемента
    Crosshair {
        anchors.centerIn: parent
        width: 48  // Размеры перекрестия
        height: 48
    }

    // Автоматическое обновление границ (оригинальная реализация)
    Binding on maxX {
        when: parent != null
        value: parent.width - root.width
    }

    Binding on maxY {
        when: parent != null
        value: parent.height - root.height
    }

    MouseArea {
		onReleased: root.dragged()
	
        anchors.centerIn: parent
		width: 48
		height: 48
        drag {
            target: root
            axis: Drag.XAndYAxis
            minimumX: root.minX
            maximumX: root.maxX
            minimumY: root.minY
            maximumY: root.maxY
        }
    }

    // Корректировка позиции (без изменений)
    onXChanged: x = Qt.binding(() => Math.max(minX, Math.min(x, maxX)))
    onYChanged: y = Qt.binding(() => Math.max(minY, Math.min(y, maxY)))
	
}