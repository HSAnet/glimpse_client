package de.hsaugsburg.informatik.mplane;

public class StorageHelper {
	public StorageHelper() {
	}
	
	private String getDataDirectory() {
                return MobileActivity.instance.getFilesDir().getAbsolutePath();
	}
	
	public String getSchedulerDirectory() {
		return getDataDirectory() + "/scheduler";
	}
	
	public String getReportDirectory() {
		return getDataDirectory() + "/reports";
	}
	
	public String getCacheDirectory() {
                return MobileActivity.instance.getCacheDir().getAbsolutePath();
	}

        public String getLogDirectory() {
                return getDataDirectory();
        }

        public String getCrashDumpDirectory() {
                return getDataDirectory() + "/crashdumps";
        }
}
