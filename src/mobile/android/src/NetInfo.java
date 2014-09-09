package de.hsaugsburg.informatik.mplane;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.net.ConnectivityManager;
import android.content.Context;
import android.net.NetworkInfo;

public class NetInfo {
    private Context context;

    public NetInfo() {
        context = MobileActivity.instance.getApplicationContext();
    }

    public int connectionMode() {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(context.CONNECTIVITY_SERVICE);
        NetworkInfo netInfo = cm.getActiveNetworkInfo();

        if (netInfo != null) {
            int netType = netInfo.getType();

            switch(netType) {
                case ConnectivityManager.TYPE_MOBILE:
                    // QNetworkInfo::GsmMode
                    return 1;
                case ConnectivityManager.TYPE_WIFI:
                    return 4;
                case ConnectivityManager.TYPE_ETHERNET:
                    return 5;
                case ConnectivityManager.TYPE_BLUETOOTH:
                    return 6;
                case ConnectivityManager.TYPE_WIMAX:
                    return 7;
                default:
                    return 0;
            }
        }

        return 0;
    }

    public boolean isOnMobileConnection() {
        // every mode besides Wifi and Ethernet is considered a mobile
        // connection
        return (connectionMode() != 4 && connectionMode() != 5);
    }
}
