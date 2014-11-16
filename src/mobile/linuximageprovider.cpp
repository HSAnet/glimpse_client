#define  WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gdk-pixdata.h>

#include "linuximageprovider.h"

LinuxImageProvider::LinuxImageProvider()
: QQuickImageProvider(Image)
{
}

LinuxImageProvider::~LinuxImageProvider()
{

}

QImage LinuxImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    WnckScreen *screen;
    GList *window_l;

    screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    // FIXME: Is there a way to get window icon without iterating?
    for (window_l = wnck_screen_get_windows(screen); window_l != NULL; window_l = window_l->next)
    {
        WnckWindow *window = WNCK_WINDOW(window_l->data);
        int winID = wnck_window_get_pid(window);

        if (!(id.toInt() == winID))
        {
            continue;
        }

        GdkPixbuf *icon = wnck_window_get_icon(window);
        gchar *buffer = 0;
        gsize buffer_size = 0;
        gdk_pixbuf_save_to_buffer(icon, &buffer, &buffer_size, "png", NULL, NULL);
        QImage pic = QImage::fromData((const uchar *)buffer, buffer_size, "png");
        return pic;
    }

    return QImage();
}
