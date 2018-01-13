package qct.mmrd.babyfacedetdmo;

import android.annotation.TargetApi;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.hardware.Camera;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.Log;


public class BFDImage {
		private final Context mContext;
	    private BFDImageRenderer mRenderer;
	    private GLSurfaceView mGlSurfaceView;
	    //private BFDGPUImageFilter mFilter;
	    private ScaleType mScaleType = ScaleType.CENTER_CROP;

	    /**    
	     * Instantiates a new GPUImage object.   
	     * 
	     * @param context the context
	     */
	    //public BFDGPUImage(final Context context, BFDGPUImageFilter imageFilter, BFDGPUImageRenderer renderer) {
	    public BFDImage(final Context context, BFDImageRenderer renderer) {
	        if (!supportsOpenGLES2(context)) {
	            throw new IllegalStateException("OpenGL ES 2.0 is not supported on this phone.");
	        }
	 
	        mContext = context; 
	        //mFilter = imageFilter;
	        mRenderer = renderer;
	    }

	    /**
	     * Checks if OpenGL ES 2.0 is supported on the current device.
	     * 
	     * @param context the context
	     * @return true, if successful
	     */
	    private boolean supportsOpenGLES2(final Context context) {
	        final ActivityManager activityManager = (ActivityManager)
	                context.getSystemService(Context.ACTIVITY_SERVICE);
	        final ConfigurationInfo configurationInfo =
	                activityManager.getDeviceConfigurationInfo();
	        return configurationInfo.reqGlEsVersion >= 0x20000;
	    }

	    /**
	     * Sets the GLSurfaceView which will display the preview.
	     * 
	     * @param view the GLSurfaceView
	     */
	    public void setGLSurfaceView(final GLSurfaceView view) {
	        mGlSurfaceView = view;
	        mGlSurfaceView.setEGLContextClientVersion(2);
	        mGlSurfaceView.setRenderer(mRenderer);
	       // mGlSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	       // mGlSurfaceView.requestRender();
	    }

	    /**
	     * Request the preview to be rendered again.
	     */
	    public void requestRender() {
	        if (mGlSurfaceView != null) {
	            mGlSurfaceView.requestRender();
	            Log.d("BFD_GPUImage:","mGlSurfaceView.requestRender");
	        }
	        
	    }

	    /**
	     * Sets the up camera to be connected to GPUImage to get a filtered preview.
	     * 
	     * @param camera the camera
	     */
	    public void setUpCamera(final Camera camera) {
	        setUpCamera(camera, 0, false, false);
	    }

	    /**
	     * Sets the up camera to be connected to GPUImage to get a filtered preview.
	     * 
	     * @param camera the camera
	     * @param degrees by how many degrees the image should be rotated
	     * @param flipHorizontal if the image should be flipped horizontally
	     * @param flipVertical if the image should be flipped vertically
	     */
	    public void setUpCamera(final Camera camera, final int degrees, final boolean flipHorizontal,
	            final boolean flipVertical) {
	        //mGlSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
	    	mGlSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	        
	        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.GINGERBREAD_MR1) {
	            setUpCameraGingerbread(camera);
	        } else {
	           // todo not support  android version < GINGERBREAD_MR1
	          Log.e("BFD_GPUImage:","ANDROID VERSION NOT SUPPORTED");
	        }
	        Rotation rotation = Rotation.NORMAL;
	        switch (degrees) {
	            case 90:
	                rotation = Rotation.ROTATION_90;
	                break;
	            case 180:
	                rotation = Rotation.ROTATION_180;
	                break;
	            case 270:
	                rotation = Rotation.ROTATION_270;
	                break;
	        }
	        mRenderer.setRotationCamera(rotation, flipHorizontal, flipVertical);
	    }

	    @TargetApi(11)
	    private void setUpCameraGingerbread(final Camera camera) {
	    	Log.d("BFD_GPUImage:","setUpCameraGingerbread++");
	    	mRenderer.setUpSurfaceTexture(camera);
	        Log.d("BFD_GPUImage:","setUpCameraGingerbread--");
	    }

	    /**
	     * Sets the filter which should be applied to the image which was (or will
	     * be) set by setImage(...).
	     * 
	     * @param filter the new filter
	     */
	    public void setFilter(final BFDImageFilter filter) {
	        //mFilter = filter;
	        //mRenderer.setFilter(mFilter);
	    	mRenderer.setFilter(filter);
	        requestRender();
	    }

	       
	    public enum ScaleType { CENTER_INSIDE, CENTER_CROP }

	}

