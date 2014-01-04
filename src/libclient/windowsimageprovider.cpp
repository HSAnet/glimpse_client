#include "windowsimageprovider.h"

WindowsImageProvider::WindowsImageProvider()
: QQuickImageProvider(Image)
{
}

WindowsImageProvider::~WindowsImageProvider()
{

}

QImage WindowsImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    return QImage();
}
