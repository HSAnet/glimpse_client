package de.hsaugsburg.informatik.mplane;

import android.graphics.*;
import android.graphics.Bitmap.Config;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.content.pm.*;
import android.content.pm.PackageManager.NameNotFoundException;

public class ImageHelper {
	public ImageHelper() {
	}
	
	public Bitmap loadApplicationIcon(String packageName) {
                PackageManager pm = MobileActivity.instance.getPackageManager();
		
		try {
			Drawable drawableIcon = pm.getApplicationIcon(packageName);
			
			// We might get the short way
			if (drawableIcon instanceof BitmapDrawable)
				return ((BitmapDrawable)drawableIcon).getBitmap();
			
			Bitmap bm = Bitmap.createBitmap(drawableIcon.getIntrinsicWidth(), drawableIcon.getIntrinsicHeight(), Config.ARGB_8888);
			Canvas canvas = new Canvas(bm);
			drawableIcon.setBounds(0, 0, bm.getWidth(), bm.getHeight());
			drawableIcon.draw(canvas);
			return bm;
		}
		catch(NameNotFoundException e) {
			return null;
		}
	}
}
