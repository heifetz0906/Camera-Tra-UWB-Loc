package qct.mmrd.babyfacedetdmo;

import java.io.InputStream;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.LinkedList;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.PointF;
import android.opengl.GLES20;
import android.os.SystemClock;
import android.util.Log;


public class BFDImageFilter {
	    // FPS
	 	private final int FPS_NO = 100;
		private long fpsData[] = new long[FPS_NO];
		private float fps = 0;
		private int fpsCount = 0;
		private int fpsDataIdx = 0;

		private void updateFPS()
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
	    
	    //texture from frame t-1
		 public int mTextureYCoordAttrib;
		 public int mTextureYUniform;
		 public int mTextureYId = OpenGlUtils.NO_TEXTURE;
		 private ByteBuffer mTextureYCoordBuf;
		 
		 public int mTextureUVCoordAttrib;
		 public int mTextureUVUniform;
		 public int mTextureUVId = OpenGlUtils.NO_TEXTURE;
		 private ByteBuffer mTextureUVCoordBuf;
		 
		 //shader to do YUV2RGBA conversion for textureYUV 
		 private static final String STACK_YUV_FILTER_VERTEX_SHADER = 
				    "attribute vec4 position;\n" +
		            "attribute vec4 inputTextureCoordinateY;\n" +
		            " \n" +
		            "varying vec2 textureCoordinateY;\n" +
		            " \n" +
		            " precision highp float;\n"+
		            " void main()\n" +
		            "{\n" +
		            "    gl_Position = position;\n" +
		            "    textureCoordinateY = inputTextureCoordinateY.xy ;\n" +
		            "}";
		 
		 public static final String STACK_YUV_FILTER_FRAGMENT_SHADER =
				    " varying vec2 textureCoordinateY;\n" +
				    " \n" +
		            " uniform sampler2D inputImageTextureY;\n" +
		            " uniform sampler2D inputImageTextureUV;\n" +
		            "\n"+
		            " precision highp float;\n" +
		            " void main()\n" +
		            " {\n" +
		            "     float yFinal = texture2D(inputImageTextureY, textureCoordinateY).r;\n" +
	                "     float uFinal = texture2D(inputImageTextureUV, textureCoordinateY).a;\n" +
	                "     float vFinal = texture2D(inputImageTextureUV, textureCoordinateY).r;\n" +
	                "     vec3 yuvFinal = vec3( 1.1643 * (yFinal-0.0627), uFinal -0.5, vFinal -0.5);\n "+
		            "     vec3 rgbFinal;\n "+
	                "     rgbFinal.r = yuvFinal.x + 1.5958*yuvFinal.z;\n"+
		            "     rgbFinal.g = yuvFinal.x - 0.39173*yuvFinal.y- 0.81290*yuvFinal.z;\n"+
	                "     rgbFinal.b = yuvFinal.x + 2.017*yuvFinal.y;\n"+
		            "     gl_FragColor = vec4(rgbFinal, 1);\n" +	            
	                "  }";	 

	    private final LinkedList<Runnable> mRunOnDraw;
	    private final String mVertexShader;
	    private final String mFragmentShader;
	    protected int mGLProgId;
	    protected int mGLAttribPosition;
	    protected int mGLUniformTexture;
	    protected int mGLAttribTextureCoordinate;
	    private int mOutputWidth;
	    private int mOutputHeight;
	    private boolean mIsInitialized=false;
	    
	    

	    public BFDImageFilter() {
	    	mRunOnDraw = new LinkedList<Runnable>();
	        mVertexShader = STACK_YUV_FILTER_VERTEX_SHADER;
	        mFragmentShader = STACK_YUV_FILTER_FRAGMENT_SHADER;
	        setRotation(Rotation.ROTATION_90, false, false);
	    }

	    public final void init() {
	        onInit();
	        //mIsInitialized = true;
	       // onInitialized();
	    }

	    public void onInit() {
	        mGLProgId = OpenGlUtils.loadProgram(mVertexShader, mFragmentShader);
	        mGLAttribPosition = GLES20.glGetAttribLocation(mGLProgId, "position");
	        mGLUniformTexture = GLES20.glGetUniformLocation(mGLProgId, "inputImageTexture");
	        mGLAttribTextureCoordinate = GLES20.glGetAttribLocation(mGLProgId,
	                "inputTextureCoordinate");
	       
	        mTextureYCoordAttrib = GLES20.glGetAttribLocation(getProgram(), "inputTextureCoordinateY");
	        mTextureYUniform = GLES20.glGetUniformLocation(getProgram(), "inputImageTextureY"); 
	        GLES20.glEnableVertexAttribArray(mTextureYCoordAttrib);
	        mTextureUVUniform = GLES20.glGetUniformLocation(getProgram(), "inputImageTextureUV"); 
	        
	    }

	    public final void destroy() {
	    	Log.d("BFD_GPUImageFilter"," destroy ++ ");
	    	mIsInitialized = false;
	        GLES20.glDeleteProgram(mGLProgId);
	        onDestroy();
	        Log.d("BFD_GPUImageFilter"," destroy -- ");
	    }

	    public void onDestroy() {
	    	Log.d("BFD_GPUImageFilter"," onDestroy ++ ");
	    	GLES20.glDeleteTextures(1, new int[]{ mTextureYId   }, 0);
	        mTextureYId = OpenGlUtils.NO_TEXTURE;
	    
	        GLES20.glDeleteTextures(1, new int[]{ mTextureUVId  }, 0);
	        mTextureUVId = OpenGlUtils.NO_TEXTURE;
	        
	         Log.d("BFD_GPUImageFilter"," onDestroy -- ");
	       
	    }

	    public void ReadPixels(Buffer  mCameraBuffer, int width, int height)
	    {   long startR= SystemClock.elapsedRealtime();
	    	GLES20.glReadPixels(0, 0, width, height,
	        		GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, mCameraBuffer);
	    	long endR1 = SystemClock.elapsedRealtime()-startR;
	           Log.d("BFD_GPUImageFilter","readpixel "+ endR1  +" ms");
	      
	    }
	    
	    public void onOutputSizeChanged(final int width, final int height) {
	        mOutputWidth = width;
	        mOutputHeight = height;
	    }

	  
	    //
	    public void onDrawYUV(final int textureIdY, final int textureIdUV, //texture t-1
	    		final FloatBuffer cubeBuffer,     final FloatBuffer textureBuffer
	    		) {

	    	Log.d("BFD_GPUImageFilter"," onDrawYUV ++");
	    	long start= SystemClock.elapsedRealtime(); 
	    	
	       
		// if(!mIsInitialized)  
		// {
	    	GLES20.glEnableVertexAttribArray(mTextureYCoordAttrib);
			mTextureYCoordBuf.position(0);	        
		    GLES20.glVertexAttribPointer(mTextureYCoordAttrib, 2, GLES20.GL_FLOAT, false, 0, mTextureYCoordBuf);
		    
		    
	        GLES20.glUniform1i(mTextureYUniform, 2);
	        GLES20.glUniform1i(mTextureUVUniform, 3);
	       
	     //   mIsInitialized = true;
	        Log.d("BFD_GPUImageFilter"," set intialised to true ");
		 	
	        GLES20.glUseProgram(mGLProgId);
	        
	     // }  
		 
	        mTextureYId = textureIdY;
		    mTextureUVId = textureIdUV;
		    
		   
		    GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
	        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureYId);
	        
		    GLES20.glActiveTexture(GLES20.GL_TEXTURE3);
	        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureUVId);
	            
	        
		   	runPendingOnDrawTasks();
	        	        
	    	cubeBuffer.position(0);
	        GLES20.glVertexAttribPointer(mGLAttribPosition, 2, GLES20.GL_FLOAT, false, 0, cubeBuffer);
	        GLES20.glEnableVertexAttribArray(mGLAttribPosition);
	        textureBuffer.position(0);
	        GLES20.glVertexAttribPointer(mGLAttribTextureCoordinate, 2, GLES20.GL_FLOAT, false, 0,
	                textureBuffer);
	        GLES20.glEnableVertexAttribArray(mGLAttribTextureCoordinate);
	        
	        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
	        
	        GLES20.glDisableVertexAttribArray(mGLAttribPosition);
	        GLES20.glDisableVertexAttribArray(mGLAttribTextureCoordinate);
	        //GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
	        
	        /*float [] cubeBuffArr =new float[8]; 
	        float [] textureBuffArr =new float [8];
	        cubeBuffer.get(cubeBuffArr).position(0);
	        textureBuffer.get(textureBuffArr).position(0);
	        Log.d("BFDGPUImage"," onDrawYUV : cubeBuff: "+cubeBuffArr[0]+","+cubeBuffArr[1]+","+cubeBuffArr[2]+","+cubeBuffArr[3]
		    		+","+cubeBuffArr[4]+","+cubeBuffArr[5]+","+cubeBuffArr[6]+","+cubeBuffArr[7]);
	        Log.d("BFDGPUImage"," onDrawYUV : textureBuff: "+textureBuffArr[0]+","+textureBuffArr[1]+","+textureBuffArr[2]+","+cubeBuffArr[3]
		    		+","+textureBuffArr[4]+","+textureBuffArr[5]+","+textureBuffArr[6]+","+textureBuffArr[7]);
	        */
	       	        
	        updateFPS();
	        long end = SystemClock.elapsedRealtime() -start;
	        Log.d("BFD_GPUImageFilter"," onDrawYUV -- execute "+ end+" ms");
	    };
	    
	    protected void runPendingOnDrawTasks() {
	        while (!mRunOnDraw.isEmpty()) {
	            mRunOnDraw.removeFirst().run();
	        }
	    }

	    public boolean isInitialized() {
	        return mIsInitialized;
	    }

	    public int getOutputWidth() {
	        return mOutputWidth;
	    }

	    public int getOutputHeight() {
	        return mOutputHeight;
	    }

	    public int getProgram() {
	        return mGLProgId;
	    }

	    public int getAttribPosition() {
	        return mGLAttribPosition;
	    }

	    public int getAttribTextureCoordinate() {
	        return mGLAttribTextureCoordinate;
	    }

	    public int getUniformTexture() {
	        return mGLUniformTexture;
	    }

	    protected void setInteger(final int location, final int intValue) {
	        runOnDraw(new Runnable() {
	            @Override
	            public void run() {
	                GLES20.glUniform1i(location, intValue);
	            }
	        });
	    }

	    protected void setFloat(final int location, final float floatValue) {
	        runOnDraw(new Runnable() {
	            @Override
	            public void run() {
	                GLES20.glUniform1f(location, floatValue);
	            }
	        });
	    }

	    protected void setFloatVec2(final int location, final float[] arrayValue) {
	        runOnDraw(new Runnable() {
	            @Override
	            public void run() {
	                GLES20.glUniform2fv(location, 1, FloatBuffer.wrap(arrayValue));
	            }
	        });
	    }

	    protected void setFloatVec3(final int location, final float[] arrayValue) {
	        runOnDraw(new Runnable() {
	            @Override
	            public void run() {
	                GLES20.glUniform3fv(location, 1, FloatBuffer.wrap(arrayValue));
	            }
	        });
	    }

	    protected void setFloatVec4(final int location, final float[] arrayValue) {
	        runOnDraw(new Runnable() {
	            @Override
	            public void run() {
	                GLES20.glUniform4fv(location, 1, FloatBuffer.wrap(arrayValue));
	            }
	        });
	    }

	    protected void setFloatArray(final int location, final float[] arrayValue) {
	        runOnDraw(new Runnable() {
	            @Override
	            public void run() {
	                GLES20.glUniform1fv(location, arrayValue.length, FloatBuffer.wrap(arrayValue));
	            }
	        });
	    }

	    protected void setPoint(final int location, final PointF point) {
	        runOnDraw(new Runnable() {

	            @Override
	            public void run() {
	            	float[] vec2 = new float[2];
	            	vec2[0] = point.x;
	            	vec2[1] = point.y;
	                GLES20.glUniform2fv(location, 1, vec2, 0);
	            }
	        });
	    }

	    protected void setUniformMatrix2f(final int location, final float[] matrix) {
	        runOnDraw(new Runnable() {

	            @Override
	            public void run() {
	                GLES20.glUniformMatrix2fv(location, 1, false, matrix, 0);
	            }
	        });
	    }

	    protected void setUniformMatrix3f(final int location, final float[] matrix) {
	        runOnDraw(new Runnable() {

	            @Override
	            public void run() {
	                GLES20.glUniformMatrix3fv(location, 1, false, matrix, 0);
	            }
	        });
	    }

	    protected void setUniformMatrix4f(final int location, final float[] matrix) {
	        runOnDraw(new Runnable() {

	            @Override
	            public void run() {
	                GLES20.glUniformMatrix4fv(location, 1, false, matrix, 0);
	            }
	        });
	    }

	    protected void runOnDraw(final Runnable runnable) {
	        synchronized (mRunOnDraw) {
	            mRunOnDraw.addLast(runnable);
	        }
	    }

	    public static String loadShader(String file, Context context) {
	        try {
	            AssetManager assetManager = context.getAssets();
	            InputStream ims = assetManager.open(file);

	            String re = convertStreamToString(ims);
	            ims.close();
	            return re;
	        } catch (Exception e) {
	            e.printStackTrace();
	        }

	        return "";
	    }

	    public static String convertStreamToString(java.io.InputStream is) {
	        java.util.Scanner s = new java.util.Scanner(is).useDelimiter("\\A");
	        return s.hasNext() ? s.next() : "";
	    }
	    
	    public void setRotation(final Rotation rotation, final boolean flipHorizontal, final boolean flipVertical) {
	        
	        float[] buffer = TextureRotationUtil.getRotation(rotation, flipHorizontal, flipVertical);
	        ByteBuffer bBuffer = ByteBuffer.allocateDirect(32).order(ByteOrder.nativeOrder());
	        FloatBuffer fBuffer = bBuffer.asFloatBuffer();
	        fBuffer.put(buffer);
	        fBuffer.flip();
	        mTextureYCoordBuf = bBuffer;
	        Log.d("BFDGPUImage"," setRotation: Buff: "+buffer[0]+","+buffer[1]+","+buffer[2]+","+buffer[3]
		    		+","+buffer[4]+","+buffer[5]+","+buffer[6]+","+buffer[7]);
	        
	        
	       	        
	    }

	}


