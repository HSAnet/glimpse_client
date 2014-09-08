package de.hsaugsburg.informatik.mplane;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import android.os.Build;
import android.os.StatFs;
import android.content.ContentResolver;
import android.provider.Settings.Secure;
import java.io.*;

import android.util.Log;

public class DeviceInfo {
    public DeviceInfo() {
    }

    public String getAndroidId() {
        ContentResolver resolver = MobileActivity.instance.getApplicationContext().getContentResolver();
        return Secure.getString(resolver, Secure.ANDROID_ID);
    }

    public String getBuildSerial() {
        return Build.SERIAL;
    }

    public float getCpuUsage() { // no longer used at the moment as this can be done in c++
        try {
            RandomAccessFile reader = new RandomAccessFile("/proc/stat", "r");
            String load = reader.readLine();

            String[] toks = load.split(" ");

            long idle1 = Long.parseLong(toks[5]);
            long cpu1 = Long.parseLong(toks[2]) + Long.parseLong(toks[3]) + Long.parseLong(toks[4])
                  + Long.parseLong(toks[6]) + Long.parseLong(toks[7]) + Long.parseLong(toks[8]);

            try {
                Thread.sleep(360);
            } catch (Exception e) {}

            reader.seek(0);
            load = reader.readLine();
            reader.close();
            toks = load.split(" ");

            long idle2 = Long.parseLong(toks[5]);
            long cpu2 = Long.parseLong(toks[2]) + Long.parseLong(toks[3]) + Long.parseLong(toks[4])
                + Long.parseLong(toks[6]) + Long.parseLong(toks[7]) + Long.parseLong(toks[8]);

            return (float)(cpu2 - cpu1) / ((cpu2 + idle2) - (cpu1 + idle1));

        } catch (IOException ex) {
            Log.e("mPlane", ex.toString());
        }

        return 0;
    }

    public String getOSName() {
        return "Android";
    }

    public String getOSVersion() {
        return Build.VERSION.RELEASE;
    }

    public String getFirmwareVersion() {
        return Build.VERSION.RELEASE;
    }

    public String getBoard() {
        return Build.BOARD;
    }

    public String getManufacturer() {
        return Build.MANUFACTURER;
    }

    public String getModel() {
        return Build.MODEL;
    }

    public long getAvailableDiskSpace() {
        // data dir
        StatFs info = new StatFs(Environment.getDataDirectory().getPath());
        long diskSpace = info.getAvailableBytes();

        // root dir
        info.restart(Environment.getRootDirectory().getPath());
        diskSpace += info.getAvailableBytes();

        return diskSpace;
    }
}
