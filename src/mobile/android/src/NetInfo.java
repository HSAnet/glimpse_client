package de.hsaugsburg.informatik.mplane;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.net.ConnectivityManager;
import android.content.Context;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiInfo;
import android.telephony.*;
import java.lang.Math;

public class NetInfo {
    private Context context;

    public NetInfo() {
        context = MobileActivity.instance.getApplicationContext();
    }

    public int connectionMode() {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(context.CONNECTIVITY_SERVICE);

        if (cm == null) {
            return 0;
        }

        NetworkInfo netInfo = cm.getActiveNetworkInfo();

        if (netInfo != null) {
            int netType = netInfo.getType();

            switch(netType) {
                case ConnectivityManager.TYPE_MOBILE:
                    TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(context.TELEPHONY_SERVICE);

                    if (telephonyManager == null) {
                        return 0;
                    }

                    int mobileNetworkType = telephonyManager.getNetworkType();

                    switch(mobileNetworkType) {
                        case TelephonyManager.NETWORK_TYPE_CDMA:
                            return 2;
                        case TelephonyManager.NETWORK_TYPE_LTE:
                            return 8;
                        case TelephonyManager.NETWORK_TYPE_GPRS:
                        case TelephonyManager.NETWORK_TYPE_EDGE:
                        default:
                            // QNetworkInfo::GsmMode
                            return 1;
                    }
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

    public int getSignalStrength() {
        int mode = connectionMode();

        switch(mode) {
            case 1:
                // mobile
                TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(context.TELEPHONY_SERVICE);

                if (telephonyManager == null) {
                    return 0;
                }

                CellInfoGsm cellInfoGsm = (CellInfoGsm) telephonyManager.getAllCellInfo().get(0);
                CellSignalStrengthGsm cellSignalStrengthGsm = cellInfoGsm.getCellSignalStrength();
                return Math.round(cellSignalStrengthGsm.getLevel() * 100);
            case 4:
                // wifi
                WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);

                if (wifiManager != null) {
                    return WifiManager.calculateSignalLevel(wifiManager.getConnectionInfo().getRssi(), 100);
                }

                return 0;
            case 5:
                // ethernet
                ConnectivityManager cm = (ConnectivityManager) context.getSystemService(context.CONNECTIVITY_SERVICE);

                if (cm == null) {
                    return 0;
                }

                NetworkInfo netInfo = cm.getActiveNetworkInfo();

                return netInfo.isConnected() ? 100 : 0;
            default:
                return 0;
        }
    }
}
