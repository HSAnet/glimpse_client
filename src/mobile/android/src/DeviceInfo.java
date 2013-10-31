package de.hsaugsburg.informatik.mplane;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import android.os.Build;
import android.content.ContentResolver;
import android.provider.Settings.Secure;

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
}
