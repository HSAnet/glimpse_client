#ifndef LINUXIMAGEPROVIDER_H
#define LINUXIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QMap>
#include <QImage>
#include <QString>

class LinuxImageProvider : public QQuickImageProvider
{
public:
    LinuxImageProvider();
    ~LinuxImageProvider();

    // QQuickImageProvider interface
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // LINUXIMAGEPROVIDER_H
