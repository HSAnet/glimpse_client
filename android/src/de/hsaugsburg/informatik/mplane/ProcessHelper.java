package de.hsaugsburg.informatik.mplane;

import org.qtproject.qt5.android.bindings.QtActivity;
import java.util.ArrayList;
import java.util.List;
import android.Manifest;
import android.os.Build;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;

@TargetApi(Build.VERSION_CODES.CUPCAKE)
public class ProcessHelper {
	public class ProcessInfo {
		public String packageName;
		public String displayName;
	}
	
	public ProcessHelper() {
	}
	
	public List<ProcessInfo> runningProcesses() {
		List<ProcessInfo> processes = new ArrayList<ProcessInfo>();
		
		ActivityManager activityManager = (ActivityManager)QtActivity.instance.getSystemService( Activity.ACTIVITY_SERVICE );
        List<RunningAppProcessInfo> procInfos = activityManager.getRunningAppProcesses();
        
        PackageManager packageManager = QtActivity.instance.getPackageManager();
        
        for (RunningAppProcessInfo runningAppProcessInfo : procInfos) {
			try {
				PackageInfo pi = packageManager.getPackageInfo(runningAppProcessInfo.processName, 0);
				pi.applicationInfo.loadLabel(packageManager);
				if (PackageManager.PERMISSION_GRANTED == packageManager.checkPermission(Manifest.permission.INTERNET, pi.packageName)) {
					// Hold the information for later use
					ProcessInfo info = new ProcessInfo();
					info.packageName = pi.packageName;
					info.displayName = packageManager.getApplicationLabel(pi.applicationInfo).toString();
					
					processes.add(info);
				} else {
					// We ignore applications without network permissions
				}
			}	
			catch (NameNotFoundException e) {
			}
        }
		
		return processes;
	}
}
