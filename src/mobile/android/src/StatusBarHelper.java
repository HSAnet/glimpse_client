package de.hsaugsburg.informatik.mplane;

import de.hsaugsburg.informatik.mplane.client.R;
import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.util.Log;

@TargetApi(Build.VERSION_CODES.ECLAIR)
public class StatusBarHelper {
        public StatusBarHelper() {
	}
	
	public String title = "mPlane is testing";
	public String message = "I am currently testing your connection";
	
        public void showIcon() {
		Notification notification = new Notification(R.drawable.statusicon, null, System.currentTimeMillis());
		notification.flags |= Notification.FLAG_ONGOING_EVENT;
		//notification.flags |= Notification.FLAG_FOREGROUND_SERVICE;
		
                PendingIntent pendingIntent = PendingIntent.getActivity(MobileActivity.instance, 0, new Intent(), 0);
                notification.setLatestEventInfo(MobileActivity.instance, title, message, pendingIntent);
	
                ((NotificationManager)MobileActivity.instance.getSystemService(Context.NOTIFICATION_SERVICE)).notify(1, notification);
		
                Log.i("mPlane", "Notification icon posted");
	}
	
	public void hideIcon() {
                Log.i("mPlane", "Notification icon removed");
                ((NotificationManager)MobileActivity.instance.getSystemService(Context.NOTIFICATION_SERVICE)).cancel(1);
	}
}
