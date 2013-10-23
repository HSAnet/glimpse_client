package de.hsaugsburg.informatik.mplane;

import org.qtproject.qt5.android.bindings.QtActivity;

public class StorageHelper {
	public StorageHelper() {
	}
	
	private String getDataDirectory() {
		return QtActivity.instance.getFilesDir().getAbsolutePath();
	}
	
	public String getSchedulerDirectory() {
		return getDataDirectory() + "/scheduler";
	}
	
	public String getReportDirectory() {
		return getDataDirectory() + "/reports";
	}
	
	public String getCacheDirectory() {
		return QtActivity.instance.getCacheDir().getAbsolutePath();
	}
}
