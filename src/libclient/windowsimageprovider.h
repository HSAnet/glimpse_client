#ifndef WINDOWSIMAGEPROVIDER_H
#define WINDOWSIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QMap>
#include <QImage>
#include <QString>

class WindowsImageProvider : public QQuickImageProvider
{
public:
    WindowsImageProvider();
    ~WindowsImageProvider();

    // QQuickImageProvider interface
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // WINDOWSIMAGEPROVIDER_H
