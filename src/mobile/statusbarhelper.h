#ifndef STATUSBARHELPER_H
#define STATUSBARHELPER_H

#include "androidhelper.h"

#include <QObject>

class StatusBarHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

public:
    explicit StatusBarHelper(QObject *parent = 0);
    ~StatusBarHelper();

    void setMessage(const QString &message);
    QString message() const;

    void setVisible(bool visible);
    bool isVisible() const;

signals:
    void messageChanged(const QString &message);
    void visibleChanged(bool visible);

protected:
    void showIcon();
    void hideIcon();

protected:
    jclass m_class;
    jobject m_instance;
    jmethodID m_showIcon;
    jmethodID m_hideIcon;

    jfieldID m_message;
    bool m_isVisible;
};

#endif // STATUSBARHELPER_H
