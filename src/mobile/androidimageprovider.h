#ifndef ANDROIDIMAGEPROVIDER_H
#define ANDROIDIMAGEPROVIDER_H

#include "androidhelper.h"

#include <QQuickImageProvider>

class AndroidImageProvider : public QQuickImageProvider
{
public:
    explicit AndroidImageProvider();
    ~AndroidImageProvider();

    // QQuickImageProvider interface
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

protected:
    jobject m_instance;
    jmethodID m_loadApplicationIcon;

    jmethodID m_getWidth;
    jmethodID m_getHeight;
    jmethodID m_getPixels;
};

#endif // ANDROIDIMAGEPROVIDER_H
