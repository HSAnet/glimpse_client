package de.hsaugsburg.informatik.mplane;

import org.qtproject.qt5.android.bindings.QtActivity;

public class MobileActivity extends QtActivity {
    public static MobileActivity instance = null;

    public MobileActivity() {
        instance = this;
    }
}
