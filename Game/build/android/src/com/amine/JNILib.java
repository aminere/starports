
package com.amine;

public class JNILib
 {
     static
	 {
         System.loadLibrary("starport");
     }
  
     public static native void init(Object activityObject, Object assetManager, String internalPath, String externalPath);
     public static native void step();
	 public static native void setTouchState(float x, float y, boolean bPressed);
	 public static native void setBackPressed(boolean bPressed);
	 public static native void createContext();
	 public static native void resizeScreen(int width, int height);
	 public static native void pause();
	 public static native void resume();
	 public static native void destroy(); 
	 public static native void stop();
	 public static native boolean isExitRequested();
	 public static native void onFPSModeAcquired();
	 public static native boolean isBuyFPSModeRequested();
	 public static native boolean resetBuyFPSModeRequest();	
	 public static native void zoom(boolean bPressed);
	 public static native void zoomStop();
}

