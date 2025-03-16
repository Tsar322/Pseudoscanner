#include "app_logic.h"


QHash<int, QByteArray> AppLogic::roleNames() const {
    return {
        {OriginalImageRole, "originalImage"},
        {ProcessedImageRole, "processedImage"},
        {NameRole, "name"}
    };
}

ImageData::ImageData(QImage originalImage_, QImage processedImage_, QString path_, cv::Mat img_)
    :originalImage(originalImage_), processedImage(processedImage_), path(path_), img(img_) {

}

AppLogic::AppLogic(QObject* parent)
	: QAbstractListModel(parent) {

}

void AppLogic::openImages(const QList<QString>& paths) {
    qDebug() << "in openImages";
    beginResetModel();
	for (const auto& path : paths) {
        qDebug() << "In openImages: path = " << path;
        cv::Mat mat = cv::imread(path.toStdString().substr(8), cv::IMREAD_COLOR); // prefix "file:///" removed
        QImage originalImage = cvMatToQImage(mat);
        QImage processedImage = originalImage.convertToFormat(QImage::Format_Grayscale8);
        
        imageData.emplaceBack(originalImage, processedImage, path, mat);
	}
    m_cursor = imageData.size() - 1;
    endResetModel();
    emit cursorDataChanged();
}

QImage AppLogic::originalImage(int i) const {
    if (i < 0 || i >= imageData.size() || imageData.size() == 0) {
        return QImage();
    }

    return imageData[i].originalImage;
}

QImage AppLogic::processedImage(int i) const {
    if (i < 0 || i >= imageData.size() || imageData.size() == 0) {
        return QImage();
    }

    return imageData[i].processedImage;
}

QString AppLogic::path(int i) const {
    if (i < 0 || i >= imageData.size() || imageData.size() == 0) {
        return QString("invalid");
    }

    return imageData[i].path;
}

int AppLogic::cursor() const {
    return m_cursor;
}

int AppLogic::imageDataSize() const {
    return imageData.size();
}

void AppLogic::setCursor(int cursor) {
    if (m_cursor != cursor) {
        m_cursor = cursor;
        emit cursorDataChanged();
    }
}

// QAbstractListModel overrides
int AppLogic::rowCount(const QModelIndex& parent) const {
    return imageData.size();
}
QVariant AppLogic::data(const QModelIndex& index, int role) const {
    if (index.row() < 0 || index.row() >= imageData.size())
        return QVariant();
    
    if (role == Qt::DisplayRole || role == NameRole) {
        return imageData[index.row()].path.split('/').back();
    }

    if (role == OriginalImageRole)
        return "image://images/" + QString::number(index.row()) + '/' + "original";
    if (role == ProcessedImageRole) {
        return "image://images/" + QString::number(index.row()) + '/' + "processed";
    }
}
QVariant AppLogic::headerData(int section, Qt::Orientation orientation, int role) const {
    return QString("header");
}


QImage cvMatToQImage(const cv::Mat& mat) {
    switch (mat.type()) {
    case CV_8UC1: {
        QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
        return image.copy();
    }
    case CV_8UC3: {
        QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_BGR888);
        return image.copy();
    }
    case CV_8UC4: {
        QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_ARGB32);
        return image.copy();
    }
    default:
        qWarning("Unsupported cv::Mat type: %d", mat.type());
        return QImage();
    }
}