package de.hsaugsburg.informatik.mplane;

import org.qtproject.qt5.android.bindings.QtActivity;
import java.lang.Thread;
import java.util.ArrayList;
import java.util.List;
import android.content.Context;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;

public class WifiLookup {
    private Context context;

    public WifiLookup() {
        context = MobileActivity.instance.getApplicationContext();
    }

    public String[] getWifiList() {
        StringBuilder sb = new StringBuilder();
        List<String> out = new ArrayList<String>();
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        if (wifiManager == null) {
            return null;
        }

        List<ScanResult> wifiList = wifiManager.getScanResults();

        for (ScanResult s: wifiList) {
            out.add(s.toString());
        }

        return out.toArray(new String[out.size()]);
    }
}
