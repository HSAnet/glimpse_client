#ifndef STATUSBARHELPER_H
#define STATUSBARHELPER_H

#include "androidhelper.h"

#include <QObject>

class StatusBarHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)

public:
    explicit StatusBarHelper(QObject *parent = 0);
    ~StatusBarHelper();

    void setMessage(const QString& message);
    QString message() const;

public slots:
    void showIcon();
    void hideIcon();

signals:
    void messageChanged(const QString& message);

protected:
    jclass m_class;
    jobject m_instance;
    jmethodID m_showIcon;
    jmethodID m_hideIcon;

    jfieldID m_message;
};

#endif // STATUSBARHELPER_H
