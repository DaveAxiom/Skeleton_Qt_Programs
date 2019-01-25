#include <QApplication>
#include <QSvgRenderer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QFileDialog>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

class SVGItem : public QGraphicsItem
{
public:
    SVGItem(QGraphicsItem* parent, QString svg_file, QPoint origin, int width) : QGraphicsItem(parent)
    {
        m_svg_file = svg_file;
        m_origin = origin;
        m_width = width;

        //Calculate height from width ratio
        QSvgRenderer svg_render(svg_file);
        QSize svg_size = svg_render.defaultSize();
        if (svg_size.width() == 0)
        {
            m_adjustment_height = 0;
            return;
        }
        float ratio = (float)svg_size.height() / (float)svg_size.width();
        m_adjustment_height = (int)((float)m_width * ratio);
    }

    QRectF boundingRect() const
    {
        return QRect(m_origin.x(), m_origin.y(), m_origin.x() + m_width, m_origin.y() + m_adjustment_height);
    }

private:
    QString m_svg_file;
    QPoint m_origin;
    int m_width, m_adjustment_height;

    void paint(QPainter* graphics, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
    {
        if (m_width == 0 || m_adjustment_height == 0)
            return;

        QSvgRenderer svg_render(m_svg_file);
        QSize svg_size;
        svg_size.setWidth(m_width);
        svg_size.setHeight(m_adjustment_height);

        QImage svg_image(svg_size, QImage::Format_ARGB32);
        svg_image.fill(0x00000000);
        QPainter svg_painter(&svg_image);
        svg_render.render(&svg_painter);
        graphics->drawImage(m_origin, svg_image);
    }
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGraphicsView view;
    QGraphicsScene* scene = new QGraphicsScene(&view);
    view.setScene(scene);

    QFileDialog FileDialog(&view);
    FileDialog.setAcceptMode(QFileDialog::AcceptOpen);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    FileDialog.setMimeTypeFilters(QStringList() << "image/svg+xml" << "image/svg+xml-compressed");
    FileDialog.setWindowTitle(QApplication::tr("Open SVG File"));
#else
    FileDialog.setNameFilters(QStringList() << "SVG files (*.svg)");
    FileDialog.setWindowTitle("Open SVG File");
#endif

    if (FileDialog.exec() == QDialog::Accepted)
    {
        QString file_selection = FileDialog.selectedFiles().first();
        scene->addItem(new SVGItem(NULL, file_selection, QPoint(0,0), 200));
    }

    view.show();
    return a.exec();
}
