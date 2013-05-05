#ifndef STATUSBARHELPER_H
#define STATUSBARHELPER_H

#include "androidhelper.h"

#include <QObject>

class StatusBarHelper : public QObject
{
    Q_OBJECT

public:
    explicit StatusBarHelper(QObject *parent = 0);
    ~StatusBarHelper();

public slots:
    void showIcon();
    void hideIcon();

protected:
    jclass m_class;
    jobject m_instance;
    jmethodID m_showIcon;
    jmethodID m_hideIcon;
};

#endif // STATUSBARHELPER_H
