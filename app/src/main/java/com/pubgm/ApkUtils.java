package com.pubgm;

import android.app.Application;
import android.content.Context;

public class ApkUtils extends Application {

    
  	static {
  		System.loadLibrary("Putri");
	}
  	public static native String GetAPK();
  	public static native String GetDataGlobal();
  	public static native String GetDataKorea();
  	public static native String GetDataIndia();
  	public static native String GetDataVietnam();
    public static native String GetDataTaiwan();
    
    public static void Start(final Context context) {
        ApkSignerEngine.BaseContext(context);
        ApkSignerEngine.KillBase(context);
    }
}

