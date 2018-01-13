package qct.mmrd.babyfacedetdmo;

import android.annotation.SuppressLint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView.Renderer;
import android.os.SystemClock;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static qct.mmrd.babyfacedetdmo.TextureRotationUtil.TEXTURE_NO_ROTATION;

@SuppressLint("NewApi")
public class BFDImageRenderer implements Renderer{
	private static final String TAG = "BFDImageRenderer";
	// FPS
	private final int FPS_NO = 100;
	private long fpsData[] = new long[FPS_NO];
	private float fps = 0;
	private int fpsCount = 0;
	private int fpsDataIdx = 0;

	public void updateFPS()
	{
	   	long start = fpsData[(fpsCount < FPS_NO) ? 0 : fpsDataIdx];
	   	long curTime = System.nanoTime();
	   	if(fpsCount > 5)
	   		fps = (float) (fpsCount/((curTime - start)/1000000000.0));
	   	if(fpsCount < FPS_NO)
	   		fpsCount += 1;
	   	fpsData[fpsDataIdx] = curTime;
	   	fpsDataIdx = (fpsDataIdx + 1) % FPS_NO;
	}
	   
	public void resetFPS()
	{
	   	fps = 0;
	   	fpsCount = 0;
	   	fpsDataIdx = 0;
	}
		    
	public float getFPS()
	{
	   	return fps;
	}	
	
	public static final int NO_IMAGE = -1;
	   static final float CUBE[] = {
	           /*-1.0f, -1.0f,
	           1.0f, -1.0f,
	           -1.0f, 1.0f,
	           1.0f, 1.0f,*/
	    	 1.0f, -1.0f,//-1.0f, -1.0f,
	        1.0f, 1.0f,//1.0f, -1.0f,
	        -1.0f, -1.0f,//-1.0f, 1.0f,
	        -1.0f,1.0f//1.0f, 1.0f,
	   };

	private BFDImageFilter mFilter;
    public final Object mSurfaceChangedWaiter = new Object();
	     
    private SurfaceTexture mSurfaceTexture = null;
    private final FloatBuffer mGLCubeBuffer;
    private final FloatBuffer mGLTextureBuffer;
    
    private int mOutputWidth;
    private int mOutputHeight;
    private int mImageWidth;
    private int mImageHeight;
    //private int mAddedPadding;

    private final Queue<Runnable> mRunOnDraw;
	private Rotation mRotation;
	private boolean mFlipHorizontal;
	private boolean mFlipVertical;
	private BFDImage.ScaleType mScaleType = BFDImage.ScaleType.CENTER_CROP;
	    
	    
	public static int mGLTextureYId = NO_IMAGE;
	public static int mGLTextureUVId = NO_IMAGE;
	public static int mGLTextureYId1 = NO_IMAGE;
	public static int mGLTextureUVId1 = NO_IMAGE;
	public static int mGLTextureYId2 = NO_IMAGE;
	public static int mGLTextureUVId2 = NO_IMAGE;
	   
	//private boolean mUseCamFrame = true;
	//private boolean mLoaded = false;
	    
	  	
	private ArrayBlockingQueue<Integer> mBuffFilledQueue; //idx for framebuffers available to compute transform
	private ArrayBlockingQueue<Integer> mProcessingDoneQueue; //computed transforms available to renderer
	private ArrayBlockingQueue<Integer> mBuffAvailQueue;// idx queue of buffers available to drop cam frames
	//private static int mFrameCount=0;
	byte[][] mFrameYBuffer;
	byte[][] mFrameUVBuffer;
	//ByteBuffer[] mByteBuffArrY;
	//ByteBuffer[] mByteBuffArrUV;
	int mNumOfFrameBuffer;
	//int mWidth, mHeight;
	Size mPreviewSize;
	boolean mInitialed = false;
	
	private Queue<Integer> mRenderQueue;
	public static int mRenderIdx;
		
	//for debug
	byte[] mTempBuff= null;
	
		
	private int mFrameCount;
	
    private DirectDrawer drawer ;
		
	public BFDImageRenderer(ArrayBlockingQueue<Integer> buffFilledQueue,
		   ArrayBlockingQueue<Integer> processingDoneQueue, ArrayBlockingQueue<Integer> buffAvailQueue,
		   byte[][] frameYBuffer,	byte[][] frameUVBuffer,	int numOfFrameBuffer,	int width, int height){
			
	  	mFilter = new BFDImageFilter();
	      
	    mRunOnDraw = new LinkedList<Runnable>();

	    mGLCubeBuffer = ByteBuffer.allocateDirect(CUBE.length * 4)
	              .order(ByteOrder.nativeOrder())
	              .asFloatBuffer();
	    mGLCubeBuffer.put(CUBE).position(0);
        mGLTextureBuffer = ByteBuffer.allocateDirect(TEXTURE_NO_ROTATION.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        setRotation(Rotation.NORMAL, false, false);
        //setRotation(Rotation.ROTATION_90, false, false);
       
	              
        mBuffFilledQueue = buffFilledQueue;
        mProcessingDoneQueue = processingDoneQueue;
        mBuffAvailQueue = buffAvailQueue;
        mFrameYBuffer = frameYBuffer;
        mFrameUVBuffer = frameUVBuffer;
        //mByteBuffArrY = byteYBuffer;
        //mByteBuffArrUV = byteUVBuffer;
        mNumOfFrameBuffer = numOfFrameBuffer;
        //mWidth = width; mHeight = height;
        mImageWidth = width; mImageHeight = height;
        
        float tempFloat1[] = new float[6];
        float tempFloat2[] = new float[6];
        //mFt = new FrameTransformPair(-1, tempFloat1, tempFloat2);
        mRenderQueue= new LinkedList<Integer>();
        
        Log.d("BFD_GPUImageRenderer","Constructor: TransformDoneQueue size "+mProcessingDoneQueue.size());
        Log.d("BFD_GPUImageRenderer","Constructor: mBuffFilledQueue size "+mBuffFilledQueue.size());
        Log.d("BFD_GPUImageRenderer","Constructor: mBuffAvailQueue size "+mBuffAvailQueue.size());
        Log.d("BFD_GPUImageRenderer","Constructor: W/H "+mImageWidth+"/"+mImageHeight);
    	
       // mInitialed = true;
        mFrameCount = -1;
       
    }

    @Override
    public void onSurfaceCreated(final GL10 unused, final EGLConfig config) {
        GLES20.glClearColor(0, 0, 0, 1);
        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        mFilter.init();
        
    }

    @Override
	public void onSurfaceChanged(final GL10 gl, final int width, final int height) {
	    mOutputWidth = width;
	    mOutputHeight = height;
	    Log.d("BFD_GPUImageRenderer"," mOutputWidth/ Height: "+mOutputWidth+" : "+mOutputHeight);
	     
	    GLES20.glViewport(0, 0, width, height);
	    //GLES20.glViewport(0, 0, 1920, 1080);
        GLES20.glUseProgram(mFilter.getProgram());
        mFilter.onOutputSizeChanged(width, height);
        synchronized (mSurfaceChangedWaiter) {
            mSurfaceChangedWaiter.notifyAll();
        }
    }
    

   	@SuppressLint({ "WrongCall", "NewApi" })
	@Override
    public void onDrawFrame(final GL10 gl) {
   		Log.d("BFD_GPUImageRenderer", "OnDrawFrame ++");
        GLES20.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   		GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        if (mSurfaceTexture != null) {
            mSurfaceTexture.updateTexImage();
            float[] mtx = new float[16];
            mSurfaceTexture.getTransformMatrix(mtx);
            drawer.draw(mtx);
        }

        synchronized (mRunOnDraw) {
            while (!mRunOnDraw.isEmpty()) {
                mRunOnDraw.poll().run();
            }
        }
       
        boolean isDataAvailable= false;
        long startW2= SystemClock.elapsedRealtime();
        synchronized (mRenderQueue){
           if(!mRenderQueue.isEmpty())
           {
              mRenderIdx=mRenderQueue.poll();
              isDataAvailable=true;
           }
        }
        long endW2 = SystemClock.elapsedRealtime() - startW2;
	    Log.d("BFD_GPUImageRenderer"," mutex wait get "+ endW2 +" ms ");
        
        if(isDataAvailable)
        {	long startR= SystemClock.elapsedRealtime();
    	    int idx = mRenderIdx;
    	    Log.d("BFD_GPUImageRenderer","OnDrawFrame Id "+idx);
    	    if(-1 != idx)
    	    {	        
    	        mGLTextureYId = OpenGlUtils.loadYTexture(ByteBuffer.wrap(mFrameYBuffer[idx]), mPreviewSize, mGLTextureYId);
	            mGLTextureUVId = OpenGlUtils.loadUVTexture(ByteBuffer.wrap(mFrameUVBuffer[idx]), mPreviewSize, mGLTextureUVId);
            
	    	   /*mGLTextureYId = OpenGlUtils.loadYTexture(mByteBuffArrY[idx], mPreviewSize, mGLTextureYId);
		       mGLTextureUVId = OpenGlUtils.loadUVTexture(mByteBuffArrUV[idx], mPreviewSize, mGLTextureUVId);
		      */
	    	   long endR1 = SystemClock.elapsedRealtime()-startR;
 	           Log.d("BFD_GPUImageRenderer","loadtexture "+ endR1  +" ms");
 	      
	  	       mFilter.onDrawYUV(mGLTextureYId,mGLTextureUVId, 
        			mGLCubeBuffer, mGLTextureBuffer );
        
               //for debug 1280*720
               //if(null == mTempBuff)
       	       //  mTempBuff=new byte[4*1280*720];//1920*1080];
       	       //mFilter.ReadPixels(ByteBuffer.wrap(mTempBuff),1280, 720);
               //BFDMainActivity.saveFrame(mTempBuff, 1280, 720);
	  	       //if(null == mTempBuff)
       	       //  mTempBuff=new byte[4*1920*1080];
       	       //mFilter.ReadPixels(ByteBuffer.wrap(mTempBuff),1920, 1080);
               //BFDMainActivity.saveFrame(mTempBuff, 1920, 1080);
        
        
               long endR = SystemClock.elapsedRealtime()-startR;
	           Log.d("BFD_GPUImageRenderer","Execute "+ endR  +" ms");
	    
               
              // updateFPS();
	           startR= SystemClock.elapsedRealtime();
               endR = SystemClock.elapsedRealtime()-startR;
	           Log.d("BFD_GPUImageRenderer","Recording trigger Execute "+ endR  +" ms");
              
	           //now unlock the frames used for rendering by releasing the prevId frame
               Log.d("BFD_GPUImageRenderer","to put Id to BuffAvailQueue: "+idx );
               try {
        	        long startW = SystemClock.elapsedRealtime();   
		 	        mBuffAvailQueue.put(idx);
		 	        long endW = SystemClock.elapsedRealtime() -startW;
                    Log.d("BFD_GPUImageRenderer"," mutex wait put "+ endW +" ms: "+ idx);
               } catch (InterruptedException e) {
			   // TODO Auto-generated catch block
		 	      e.printStackTrace();
		       }
               //Log.d("BFD_GPUImageRenderer","Id put to BuffAvailQueue: "+prevIdx);
    	 
               updateFPS();
    	    }// nextId =-1?
        }//isDataAvailable
        else
        {
        	Log.e("BFD_GPUImageRenderer","OnDrawFrame: Empty Queue --");   
        }
        
	    Log.d("BFD_GPUImageRenderer","OnDrawFrame --");        
    }
    
    // add a runnable task of setup camera preview texture to runOnDraw list
    public void setUpSurfaceTexture(final Camera camera) {
    	Log.d("BFD_GPUImageRenderer"," setUpSurfaceTexture add task");
    	
        runOnDraw(new Runnable() {
            @Override
            public void run() {
            	Log.d("BFD_GPUImageRenderer"," setUpSurfaceTexture ++");
            	final Size previewSize = camera.getParameters().getPreviewSize();
                mPreviewSize = previewSize;
                
                if(null==mSurfaceTexture)
                {
                int[] textures = new int[1];
                GLES20.glGenTextures(1, textures, 0);
                    GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textures[0]);
                    GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                            GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
                    GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                            GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
                    GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                            GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
                    GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
                            GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);
                mSurfaceTexture = new SurfaceTexture(textures[0]);
                drawer = new DirectDrawer(textures[0]) ;

                Log.d("BFD_GPUImageRenderer"," surfaceTexture bound with textureId "+ textures[0]);
                }
                else
                { Log.d("BFD_GPUImageRenderer"," used previous surface texture");
                }
                
                try {
                    camera.setPreviewTexture(mSurfaceTexture);
                    Log.d("BFD_GPUImageRenderer"," camera setPreviewTexture");
                } catch (IOException e) {
                    e.printStackTrace();
                }
                
                Log.d("BFD_GPUImageRenderer"," setUpSurfaceTexture --");
            }
        });
    }
    public void initialize()
    {
    	 mGLTextureYId = NO_IMAGE;
    	 mGLTextureUVId = NO_IMAGE;
    	 mGLTextureYId1 = NO_IMAGE;
    	 mGLTextureUVId1 = NO_IMAGE;
    	 mGLTextureYId2 = NO_IMAGE;
    	 mGLTextureUVId2 = NO_IMAGE;
    	   
    	  	
    }
    // initial the filter and use filter to do some configuration
    public void setFilter(final BFDImageFilter filter) {
        runOnDraw(new Runnable() {

            @Override
            public void run() {
            	Log.d("BFD_GPUImageRenderer"," setFilter ++");
                final BFDImageFilter oldFilter = mFilter;
                mFilter = filter;
                if (oldFilter != null) {
                    oldFilter.destroy();
                }
                
                mFilter.init();
                GLES20.glUseProgram(mFilter.getProgram());
                mFilter.onOutputSizeChanged(mOutputWidth, mOutputHeight);
                Log.d("BFD_GPUImageRenderer"," setFilter --");
            }
        });
    }
      
    public void setRenderIdx(int idx)
    {
    	mRenderIdx = idx;
    }
    
    public void addToQueue(int idx)
    {   long startW2 = SystemClock.elapsedRealtime();
    	synchronized(mRenderQueue)
    	{
    	  mRenderQueue.add(idx);
    	}
    	long endW2 = SystemClock.elapsedRealtime() -startW2;
        Log.d("BFD_GPUImageRenderer"," add local queue mutex wait put "+ endW2 +" ms  ");
    	
    }
    public void emptyQueue()
    {
    	synchronized(mRenderQueue)
    	{
    		mRenderQueue.clear();
    	}
    }
    
    public void setScaleType(BFDImage.ScaleType scaleType) {
        mScaleType = scaleType;
    }
    
    protected int getFrameWidth() {
       return mOutputWidth;
    }

    protected int getFrameHeight() {
        return mOutputHeight;
    }

    private void adjustImageScaling() {
    	Log.e("BFD_GPUImageRenderer"," adjustImageScaling called: mImageWidth/height: "+mImageWidth+":"+mImageHeight);
        float outputWidth = mOutputWidth;
        float outputHeight = mOutputHeight;
        if (mRotation == Rotation.ROTATION_270 || mRotation == Rotation.ROTATION_90) {
            outputWidth = mOutputHeight;
            outputHeight = mOutputWidth;
        }

        float ratio1 = outputWidth / mImageWidth;
        float ratio2 = outputHeight / mImageHeight;
        float ratioMin = Math.min(ratio1, ratio2);
        mImageWidth = Math.round(mImageWidth * ratioMin);
        mImageHeight = Math.round(mImageHeight * ratioMin);
        Log.e("BFD_GPUImageRenderer"," adjustImageScaling : mImageWidth/height: "+mImageWidth+":"+mImageHeight);
        
        float ratioWidth = 1.0f;
        float ratioHeight = 1.0f;
        if (mImageWidth != outputWidth) {
            ratioWidth = mImageWidth / outputWidth;
        } else if (mImageHeight != outputHeight) {
            ratioHeight = mImageHeight / outputHeight;
        }
        Log.e("BFD_GPUImageRenderer"," adjustImageScaling : ratioWidth/height: "+ratioWidth+":"+ratioHeight);
        
        float[] cube = CUBE;
        float[] textureCords = TextureRotationUtil.getRotation(mRotation, mFlipHorizontal, mFlipVertical);
        if (mScaleType == BFDImage.ScaleType.CENTER_CROP) {
            float distHorizontal = (1 / ratioWidth - 1) / 2;
            float distVertical = (1 / ratioHeight - 1) / 2;
            textureCords = new float[]{
                    addDistance(textureCords[0], distVertical), addDistance(textureCords[1], distHorizontal),
                    addDistance(textureCords[2], distVertical), addDistance(textureCords[3], distHorizontal),
                    addDistance(textureCords[4], distVertical), addDistance(textureCords[5], distHorizontal),
                    addDistance(textureCords[6], distVertical), addDistance(textureCords[7], distHorizontal),
            };
        } else {
            cube = new float[]{
                    CUBE[0] * ratioWidth, CUBE[1] * ratioHeight,
                    CUBE[2] * ratioWidth, CUBE[3] * ratioHeight,
                    CUBE[4] * ratioWidth, CUBE[5] * ratioHeight,
                    CUBE[6] * ratioWidth, CUBE[7] * ratioHeight,
            };
        }
        Log.e("GPUImage-ImageRenderer"," adjustImageScaling CUBE:"+CUBE[0]+","+CUBE[1]+","
        		+CUBE[2]+","+CUBE[3]+","+CUBE[4]+","+CUBE[5]+","+CUBE[6]+","+CUBE[7]+",");
        Log.e("GPUImage-ImageRenderer"," adjustImageScaling cube:"+cube[0]+","+cube[1]+","
        		+cube[2]+","+cube[3]+","+cube[4]+","+cube[5]+","+cube[6]+","+cube[7]+",");
        Log.e("GPUImage-ImageRenderer"," adjustImageScaling textCoord:"+textureCords[0]+","+textureCords[1]+","
        		+textureCords[2]+","+textureCords[3]+","+textureCords[4]+","+textureCords[5]+","+textureCords[6]+","+textureCords[7]+",");
        mGLCubeBuffer.clear();
        mGLCubeBuffer.put(cube).position(0);
        mGLTextureBuffer.clear();
        mGLTextureBuffer.put(textureCords).position(0);
    }

    private float addDistance(float coordinate, float distance) {
	        return coordinate == 0.0f ? distance : 1 - distance;
   }

    public void setRotationCamera(final Rotation rotation, final boolean flipHorizontal,
            final boolean flipVertical) {
        setRotation(rotation, flipVertical, flipHorizontal);
    }

    public void setRotation(final Rotation rotation, final boolean flipHorizontal,
           final boolean flipVertical) {
        mRotation = rotation;
        mFlipHorizontal = flipHorizontal;
        mFlipVertical = flipVertical;
        adjustImageScaling();
    }

    public Rotation getRotation() {
        return mRotation;
    }

    public boolean isFlippedHorizontally() {
        return mFlipHorizontal;
    }

    public boolean isFlippedVertically() {
        return mFlipVertical;
    }

    protected void runOnDraw(final Runnable runnable) {
        synchronized (mRunOnDraw) {
            mRunOnDraw.add(runnable);
        }
    }
}
