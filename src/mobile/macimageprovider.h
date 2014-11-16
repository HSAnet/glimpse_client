#ifndef MACIMAGEPROVIDER_H
#define MACIMAGEPROVIDER_H

#include <QQuickImageProvider>

class MacImageProvider : public QQuickImageProvider
{
public:
    MacImageProvider();
    ~MacImageProvider();

    // QQuickImageProvider interface
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // MACIMAGEPROVIDER_H
