#include "statusbarhelper.h"

StatusBarHelper::StatusBarHelper(QObject *parent)
: QObject(parent)
{
    Java env;
    m_class = env.findClass("de/hsaugsburg/informatik/mplane/StatusBarHelper");
    m_instance = env.createInstance(m_class);
    m_showIcon = env->GetMethodID(m_class, "showIcon", "()V");
    m_hideIcon = env->GetMethodID(m_class, "hideIcon", "()V");
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

namespace {
    static int init_StatusBarHelper() {
        Java::registerClass("de/hsaugsburg/informatik/mplane/StatusBarHelper");
        return 1;
    }

    static int __StatusBarHelper = init_StatusBarHelper();
}
