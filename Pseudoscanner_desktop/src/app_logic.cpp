#include "app_logic.h"


QHash<int, QByteArray> AppLogic::roleNames() const {
    return {
        {OriginalImageRole, "originalImage"},
        {ProcessedImageRole, "processedImage"},
        {NameRole, "name"}
    };
}


AppLogic::AppLogic(QObject* parent)
	: QAbstractListModel(parent) {

}

void AppLogic::openImages(const QList<QString>& paths) {
	for (const auto& path : paths) {
        cv::Mat mat = cv::imread(path.toStdString().substr(8), cv::IMREAD_COLOR); // prefix "file:///" removed
        if (!mat.empty()) {
            beginInsertRows(QModelIndex(), imageData.size(), imageData.size());
            QImage originalImage = cvMatToQImage(mat);
            QImage processedImage = originalImage.convertToFormat(QImage::Format_Grayscale8);

            imageData.emplaceBack(originalImage, processedImage, path, mat);
            endInsertRows();
            setCursor(imageData.size() - 1);
        }
	}
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
    if (m_cursor != cursor && cursor >= 0 && cursor < imageData.size()) {
        m_cursor = cursor;
        emit cursorDataChanged();
    }
}

int AppLogic::getCorner(int cursor, int corner_index, bool is_x) const {
    if (cursor >= imageData.size() || cursor < 0) {
        qDebug() << "Invalid image index";
        return 0;
    }
    if (corner_index < 0 || corner_index > 3) {
        qDebug() << "invalid corner index";
        return 0;
    }
    const cv::Point& corner = imageData[cursor].documentCorners[corner_index];
    if (is_x) return corner.x;
    else return corner.y;
}

void AppLogic::setCorner(int cursor, int corner_index, int x, int y) {
    if (cursor >= imageData.size() || cursor < 0) {
        qDebug() << "Invalid image index";
    }
    if (corner_index < 0 || corner_index > 3) {
        qDebug() << "invalid corner index";
    }
    cv::Point& corner = imageData[cursor].documentCorners[corner_index];
    if (x < 0) corner.x = 0;
    else if (x >= imageData[cursor].img.cols) corner.x = imageData[cursor].img.cols - 1;
    else corner.x = x;

    if (y < 0) corner.y = 0;
    else if (y >= imageData[cursor].img.rows) corner.y = imageData[cursor].img.rows - 1;
    else corner.y = y;
}

void AppLogic::predetectCorners(int index) {
    imageData[index].documentCorners = scanner_img_proc::predetermine_quadrangle(imageData[index].img);
}

void AppLogic::applyTransform(int index) {
    const cv::Mat& img = imageData[index].img;
    std::vector<cv::Point>& document_corners = imageData[index].documentCorners;
    cv::Mat dst;
    cv::Size img_size = img.size();
    std::vector<cv::Point> img_corners{ cv::Point(img_size.width - 1, 0), cv::Point(img_size.width - 1, img_size.height - 1), cv::Point(0, img_size.height - 1), cv::Point(0, 0) };

    if (scanner_img_proc::distribute_points(document_corners)) {
        cv::Point origin(img.cols / 2, img.rows / 2);
        double aspect_ratio = scanner_img_proc::get_aspect_ratio(document_corners, origin);
        scanner_img_proc::transform_perspective(img, dst, document_corners, aspect_ratio);
        imageData[index].processedImage = cvMatToQImage(dst);
        emit processedImageChanged();
    }
    else {
        qDebug() << "Invalid document corners";
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