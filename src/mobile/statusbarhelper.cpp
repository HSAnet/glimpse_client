#include "statusbarhelper.h"

StatusBarHelper::StatusBarHelper(QObject *parent)
: QObject(parent)
{
    Java env;
    m_class = env.findClass("de/hsaugsburg/informatik/mplane/StatusBarHelper");
    m_instance = env.createInstance(m_class);
    m_showIcon = env->GetMethodID(m_class, "showIcon", "()V");
    m_hideIcon = env->GetMethodID(m_class, "hideIcon", "()V");
    m_message = env->GetFieldID(m_class, "message", "Ljava/lang/String;");
}

StatusBarHelper::~StatusBarHelper()
{
    Java env;
    env->DeleteGlobalRef(m_instance);
}

void StatusBarHelper::showIcon()
{
    Java env;
    env->CallVoidMethod(m_instance, m_showIcon);
}

void StatusBarHelper::hideIcon()
{
    Java env;
    env->CallVoidMethod(m_instance, m_hideIcon);
}

void StatusBarHelper::setMessage(const QString &message)
{
    Java env;
    jstring str = (jstring)env->NewLocalRef(env->NewString((const jchar*)message.constData(), message.size()));
    env->SetObjectField(m_instance, m_message, str);
    env->DeleteLocalRef(str);

    emit messageChanged(message);
}

QString StatusBarHelper::message() const
{
    Java env;
    jstring str = (jstring)env->GetObjectField(m_instance, m_message);
    const jchar* chars = env->GetStringChars(str, NULL);
    QString msg = QString::fromUtf16(chars, env->GetStringLength(str));
    env->ReleaseStringChars(str, chars);
    return msg;
}

namespace {
    static int init_StatusBarHelper() {
        Java::registerClass("de/hsaugsburg/informatik/mplane/StatusBarHelper");
        return 1;
    }

    static int __StatusBarHelper = init_StatusBarHelper();
}
