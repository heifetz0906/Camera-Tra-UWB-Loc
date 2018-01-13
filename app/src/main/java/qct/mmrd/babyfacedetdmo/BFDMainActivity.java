package qct.mmrd.babyfacedetdmo;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import qct.mmrd.babyfacedetdmo.CameraHelper.CameraInfo2;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.RelativeLayout.LayoutParams;

public class BFDMainActivity extends Activity {

    private final String TAG = BFDMainActivity.this.getClass().getSimpleName();

    private final int MAX_FRAME_BUFF_NO = 6;
    static public final int MAX_NUM_FACES = 10;
    static public final int NUM_FRAMES_FILTER = 7; //num frames for filtering

    ImageButton mCameraSwitchButton;

    //GPUImage stuff
    private BFDImage mGPUImage;
    private BFDImageFilter mFilter;
    private BFDImageRenderer mRenderer;
    private CameraHelper mCameraHelper;
    private CameraLoader mCamera;
    private GLSurfaceView mGLView;
    private MyImageView mActivityView;

    //data 
    private byte[][] mFrameBuffArrY = null;
    private byte[][] mFrameBuffArrUV = null;
    private byte[] mFrameBuffYUV_LOAD = null;
    private int mFrameWidth = 640;//1280;//1920;//
    private int mFrameHeight = 480;//720;//1080;//
    private int mNumOfFrameBuff;
    private int mFrameSize;
    //ion buff
    private ByteBuffer mByteBuffArrY[] = null;
    private ByteBuffer mByteBuffArrUV[] = null;
    private ByteBuffer mByteBuffPreview;

    //for concurrency
    private ArrayBlockingQueue<Integer> mBuffFilledQueue; //idx for framebuffers available to compute transform
    private static ArrayBlockingQueue<Integer> mProcessingDoneQueue; // id queue of processing done frames that are available to renderer
    private ArrayBlockingQueue<Integer> mBuffAvailQueue;  // idx queue of buffers available to drop cam frames

    private ExecutorService mExec;
    private RenderThread mRenderThread;
    private boolean mThreadExit = false;

    private byte[] mPreviewBuff = null;

    //face det
    static private byte[] mFaceScores;
    static private int[] numFaceDet;
    static private float[] mFaceBoundingBoxes;
    static private int[] mFaceIdTracked;

    //timer for FPS display
    private Timer mFpsTimer;
    private TimerTask mFpsTimerTask;


    private BlueToothService mBTService = null;
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    boolean isConnected = false;
    private View touchFrame, systemFrame;

    private TouchEventHelper mTouchEventHelper;

    private final static String BLUETOOTH_ADDRESS = "6C:40:08:B4:3B:E5";
    
	boolean startSend = false ;//开始发送flag
    
    public static int isInit = 0 ;
//这里是显示fps的线程
    class MyTimerTask extends TimerTask {
        private BFDMainActivity mContext;

        MyTimerTask(BFDMainActivity context) {
            mContext = context;
        }

        @Override
        public void run() {
            mContext.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //int score = scores[0]&0xff;
                    ((TextView) mContext.findViewById(R.id.fpsView)).setText(
                            String.format("fps: %.2f ", BFDMainActivity.this.mFilter.getFPS()));
                    //String.format("fps: %.2f, Face #: %d , Score: %d", BFDMainActivity.this.mFilter.getFPS(), numFaceDet[0], score));
                    //String.format("camera: %.2f render: %.2f", VSMainActivity.this.mRenderer.getFPS(), VSMainActivity.this.mFilter.getFPS()));
                }
            });
        }
    };
    
    public Handler getHandler() {
		return handler;
	}

	private Handler handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case 0:
				break;
			case 1:// 扫描完毕
					// progressDialog.cancel();
				mBTService.StopScan();
				break;
			case 2:// 停止扫描
				break;
			}
		}
	};

	ConnectBlueToothDialog dialog;

	// 弹出蓝牙列表
	public void showDialog() {
		dialog = new ConnectBlueToothDialog(BFDMainActivity.this,
				R.style.confirmDialog1);
		dialog.setmBTService(mBTService);

		dialog.show();
	}

    @SuppressWarnings("unused")
    private Handler mhandler = new Handler() {
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case MESSAGE_STATE_CHANGE:// è“�ç‰™è¿žæŽ¥çŠ¶æ€�
                    switch (msg.arg1) {
                        case BlueToothService.STATE_CONNECTED:// å·²ç»�è¿žæŽ¥
                            break;
                        case BlueToothService.STATE_CONNECTING:// æ­£åœ¨è¿žæŽ¥
                            break;
                        case BlueToothService.STATE_LISTEN:
                        case BlueToothService.STATE_NONE:
                            break;
                        case BlueToothService.SUCCESS_CONNECT: // è¿žæŽ¥æˆ�åŠŸ
                            isConnected = true;
                            Toast.makeText(BFDMainActivity.this, "连接成功",
                                    Toast.LENGTH_SHORT).show();
                            dialog.dismiss() ;
                            break;
                        case BlueToothService.FAILED_CONNECT: // è¿žæŽ¥å¤±è´¥
                            Toast.makeText(BFDMainActivity.this, "连接失败",
                                    Toast.LENGTH_SHORT).show();
                            break;
                        case BlueToothService.LOSE_CONNECT:
                            isConnected = false;
                            Toast.makeText(BFDMainActivity.this, "失去连接", Toast.LENGTH_SHORT).show();
                            switch (msg.arg2) {
                                case -1:
                                    isConnected = false;
                                    break;
                                case 0:
                                    break;
                            }
                    }
                    break;
                case MESSAGE_READ:
                    try {
                        String info = (String) msg.obj;
                        Log.e(TAG, "info:======" + info);
                    } catch (Exception e) {
                        Log.e("MESSAGE_READ", e.getMessage());
                    }
                    break;
                case MESSAGE_WRITE:
                    break;

            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_bfd_main);
        
        mBTService = new BlueToothService(this, mhandler);// 创建对象的时候必须有一个是Handler类型

		if (!mBTService.IsOpen()) {
			mBTService.OpenDevice();
		}
		
		 //接受到蓝牙的时候
        mBTService.setOnReceive(new BlueToothService.OnReceiveDataHandleEvent() {

            @Override
            public void OnReceive(BluetoothDevice device) {
                // TODO Auto-generated method stub
                if (device != null) {
                    if (dialog != null) {
                        dialog.addDeviceForAdapter(device.getName() + "\n"
                                + device.getAddress());
                    }
                } else {
                    Message msg = new Message();
                    msg.what = 1;
                    handler.sendMessage(msg);

                }
            }
        });

        touchFrame = findViewById(R.id.touch_frame);
        systemFrame = findViewById(R.id.system_frame);

        mTouchEventHelper = new TouchEventHelper(touchFrame);

        mGLView = (GLSurfaceView) findViewById(R.id.surfaceView);
        mActivityView = (MyImageView) this.findViewById(R.id.activityView);
        numFaceDet = new int[1];
        mFaceScores = new byte[MAX_NUM_FACES];
        mFaceBoundingBoxes = new float[8 * MAX_NUM_FACES];
        mFaceIdTracked = new int[MAX_NUM_FACES];

        mNumOfFrameBuff = MAX_FRAME_BUFF_NO;
        mFrameSize = mFrameWidth * mFrameHeight;

        if (null == mFrameBuffArrY)
            mFrameBuffArrY = new byte[MAX_FRAME_BUFF_NO][mFrameSize];
        if (null == mFrameBuffArrUV)
            mFrameBuffArrUV = new byte[MAX_FRAME_BUFF_NO][mFrameSize / 2];

        if (null == mPreviewBuff)
            mPreviewBuff = new byte[mFrameSize + mFrameSize / 2];

        mBuffFilledQueue = new ArrayBlockingQueue<Integer>(MAX_FRAME_BUFF_NO);
        mProcessingDoneQueue = new ArrayBlockingQueue<Integer>(MAX_FRAME_BUFF_NO);
        mBuffAvailQueue = new ArrayBlockingQueue<Integer>(MAX_FRAME_BUFF_NO);

        mFilter = new BFDImageFilter();
        mRenderer = new BFDImageRenderer(mBuffFilledQueue, mProcessingDoneQueue, mBuffAvailQueue,
                mFrameBuffArrY, mFrameBuffArrUV, mNumOfFrameBuff, mFrameWidth, mFrameHeight);

        mGPUImage = new BFDImage(this, mRenderer);
        //mGPUImage.setGLSurfaceView((GLSurfaceView) findViewById(R.id.surfaceView));
        mGPUImage.setGLSurfaceView(mGLView);

        ((TextView) findViewById(R.id.fpsView)).setVisibility(View.VISIBLE);
        if (mFpsTimer != null) {
            mFpsTimer.cancel();
        }

        mCameraHelper = new CameraHelper(this);
        mCamera = new CameraLoader();
        mCameraSwitchButton = (ImageButton) this.findViewById(R.id.img_switch_camera);
        //mCameraSwitchButton.setBackgroundResource(R.drawable.ic_switch_camera);
        //View cameraSwitchView = findViewById(R.id.img_switch_camera);
        //cameraSwitchView.setOnClickListener(this);
        if (!mCameraHelper.hasFrontCamera() || !mCameraHelper.hasBackCamera()) {
            mCameraSwitchButton.setVisibility(View.GONE);
        }
        mCameraSwitchButton.setVisibility(View.GONE);

        mCameraSwitchButton.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                //empty the queue and enqueue an exit element to stop OnDrawFrame() of renderer
                int exitId = -1;
                try {
                    mProcessingDoneQueue.clear();
                    mProcessingDoneQueue.put(exitId);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                try {
                    mBuffAvailQueue.clear();
                    mBuffAvailQueue.put(exitId);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                try {
                    mBuffFilledQueue.clear();
                    mBuffFilledQueue.put(exitId);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                mThreadExit = true;
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                mExec.shutdown();

                mRenderer.emptyQueue();

//                deInitFaceDet();

                if (mFpsTimer != null)
                    mFpsTimer.cancel();

                mGLView.onPause();
                mCamera.switchCamera();

                mFpsTimer = new Timer();
                mFpsTimerTask = new MyTimerTask(BFDMainActivity.this);
                mFpsTimer.schedule(mFpsTimerTask, 1000, 1000);

                mRenderer.emptyQueue();
                if (!mProcessingDoneQueue.isEmpty())
                    mProcessingDoneQueue.clear();
                if (!mBuffFilledQueue.isEmpty())
                    mBuffFilledQueue.clear();
                if (!mBuffAvailQueue.isEmpty())
                    mBuffAvailQueue.clear();
                for (int i = 0; i < MAX_FRAME_BUFF_NO; i++) {
                    try {
                        mBuffAvailQueue.put(i);
                    } catch (InterruptedException e) {
                        Log.e(TAG, e.toString());
                    }
                }
                mRenderer.initialize();

//                initFaceDet(mFrameHeight, 48, MAX_NUM_FACES, 20);
                mThreadExit = false;
                mExec = Executors.newFixedThreadPool(2);
                mExec.execute(new BFDFrameProcessing());
                mExec.execute(new RenderThread());
                mGPUImage.setFilter(mFilter);
                mGLView.onResume();

            }
        });
        Log.d(TAG, " OnCreate--");
        
        findViewById(R.id.search_bluetooth).setOnClickListener(
				new OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						showDialog() ;
					}
				});

		findViewById(R.id.start_send).setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				startSend = !startSend ;
				((Button)v).setText(startSend ? "取消发送" :"开始发送") ;
			}
		});
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
        mTouchEventHelper.dispatchTouchEvent(ev);
        return super.dispatchTouchEvent(ev);
    }


    @Override
    protected void onResume() {
        Log.d(TAG, " OnResume ++");
        super.onResume();

        mFpsTimer = new Timer();
        mFpsTimerTask = new MyTimerTask(this);
        mFpsTimer.schedule(mFpsTimerTask, 1000, 1000);

        mRenderer.emptyQueue();
        if (!mProcessingDoneQueue.isEmpty())
            mProcessingDoneQueue.clear();
        if (!mBuffFilledQueue.isEmpty())
            mBuffFilledQueue.clear();
        if (!mBuffAvailQueue.isEmpty())
            mBuffAvailQueue.clear();
        for (int i = 0; i < MAX_FRAME_BUFF_NO; i++) {
            try {
                mBuffAvailQueue.put(i);
            } catch (InterruptedException e) {
                Log.e(TAG, e.toString());
            }
        }
        mRenderer.initialize();

//        initFaceDet(mFrameHeight, 48, MAX_NUM_FACES, 20);
        mThreadExit = false;
        mExec = Executors.newFixedThreadPool(2);
        mExec.execute(new BFDFrameProcessing());
        mExec.execute(new RenderThread());

        mCamera.onResume();
        mGPUImage.setFilter(mFilter);
        mGLView.onResume();
    }

    @Override
    protected void onPause() {

        //empty the queue and enqueue an exit element to stop OnDrawFrame() of renderer
        int exitId = -1;
        try {
            mProcessingDoneQueue.clear();
            mProcessingDoneQueue.put(exitId);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        try {
            mBuffAvailQueue.clear();
            mBuffAvailQueue.put(exitId);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        try {
            mBuffFilledQueue.clear();
            mBuffFilledQueue.put(exitId);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        mThreadExit = true;
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        mExec.shutdown();

        mRenderer.emptyQueue();

//        deInitFaceDet();

        if (mFpsTimer != null)
            mFpsTimer.cancel();
        mCamera.onPause();
        mGLView.onPause();
        super.onPause();

        Log.d(TAG, " OnPause--");


    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, " onDestroy ++");

        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        Log.d(TAG, "to release buff");
        for (int i = 0; i < MAX_FRAME_BUFF_NO; i++) {
            mFrameBuffArrY[i] = null;
            mFrameBuffArrUV[i] = null;
        }
        mFrameBuffArrY = null;
        mFrameBuffArrUV = null;

        mFrameBuffYUV_LOAD = null;
        mPreviewBuff = null;

        mFaceScores = null;
        mFaceBoundingBoxes = null;
        mFaceIdTracked = null;

        Log.d(TAG, "released buff");

        super.onDestroy();

        Log.d(TAG, " onDestroy --");

    }

    @Override
    protected void onStop() {
        Log.d(TAG, " onStop ++");
        super.onStop();
        Log.d(TAG, " onStop --");

    }

    @Override
    protected void onRestart() {
        Log.d(TAG, " onRestart ++");
        super.onRestart();
        Log.d(TAG, " onRestart --");

    }

    private class CameraLoader implements PreviewCallback {
        private int mCurrentCameraId = 0;
        private Camera mCameraInstance;
        private boolean mFirstFrame = true;

        // setup camera
        public void onResume() {
            setUpCamera(mCurrentCameraId);

            mCameraInstance.addCallbackBuffer(mPreviewBuff);
            mCameraInstance.setPreviewCallbackWithBuffer(this);
            mCameraInstance.startPreview();
        }

        public void onPause() {
            releaseCamera();
        }

        public void switchCamera() {
            releaseCamera();
            mCurrentCameraId = (mCurrentCameraId + 1) % mCameraHelper.getNumberOfCameras();
            setUpCamera(mCurrentCameraId);

            mCameraInstance.addCallbackBuffer(mPreviewBuff);
            mCameraInstance.setPreviewCallbackWithBuffer(this);
            mCameraInstance.startPreview();
            Log.d("BFD_CameraLoader", " switchCamera--");
        }

        public Camera getCamera() {
            return mCameraInstance;
        }

        @Override
        public void onPreviewFrame(final byte[] data, final Camera camera) {
            long startP = SystemClock.elapsedRealtime();
            try {
                Log.d("BFD_CameraLoader", " to get Id from BuffAvailQueue of size: " + mBuffAvailQueue.size());
                //mRenderer.updateFPS(); //use renderer's timer to count fps

                long startW = SystemClock.elapsedRealtime();
                int index = mBuffAvailQueue.take();
                long endW = SystemClock.elapsedRealtime() - startW;
                Log.d("BFD_CameraLoader", " mutex wait get " + endW + " ms: " + index);
                //Log.d("BFD_CameraLoader"," got Id from BuffAvailQueue: "+index);

                long startR = SystemClock.elapsedRealtime();
                if (false) // true for debug, load img from disk
                {
                } else {
                    {
                        System.arraycopy(data, 0, mFrameBuffArrY[index], 0, mFrameWidth * mFrameHeight);
                        System.arraycopy(data, mFrameWidth * mFrameHeight, mFrameBuffArrUV[index], 0, mFrameWidth * mFrameHeight / 2);
                    }

                }
                long endR = SystemClock.elapsedRealtime() - startR;
                Log.d("BFD_CameraLoader", " copy Execute " + endR + " ms");

                camera.addCallbackBuffer(mPreviewBuff);

                mBuffFilledQueue.put(index);


            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            long endP = SystemClock.elapsedRealtime() - startP;
            Log.d("BFD_CameraLoader", " OnPreviewFrame execute " + endP + " ms");
        }

        //setup camera: config camera, GPU render mode, cameraPreviewTexure
        @SuppressLint("NewApi")
        private void setUpCamera(final int id) {

            Log.d("BFD_CameraLoader", " setupCamera++");

            mCameraInstance = getCameraInstance(id);
            Parameters parameters = mCameraInstance.getParameters();
            // TODO adjust by getting supportedPreviewSizes and then choosing
            // the best one for screen size (best fill screen)
            parameters.setPreviewSize(mFrameWidth, mFrameHeight);
            if (parameters.getSupportedFocusModes().contains(
                    Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            }
            mCameraInstance.setParameters(parameters);
            parameters = mCamera.mCameraInstance.getParameters();


            int orientation = mCameraHelper.getCameraDisplayOrientation(
                    BFDMainActivity.this, mCurrentCameraId);
            CameraInfo2 cameraInfo = new CameraInfo2();
            mCameraHelper.getCameraInfo(mCurrentCameraId, cameraInfo);
            boolean flipHorizontal = cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT
                    ? true : false;
            mGPUImage.setUpCamera(mCameraInstance, orientation, flipHorizontal, false);
        }

        /**
         * A safe way to get an instance of the Camera object.
         */
        private Camera getCameraInstance(final int id) {
            Camera c = null;
            try {
                c = mCameraHelper.openCamera(id);
            } catch (Exception e) {
                e.printStackTrace();
            }
            return c;
        }

        private void releaseCamera() {
            Log.d("BFD_CameraLoader", "  release camera ++");
            mCameraInstance.addCallbackBuffer(null);
            mCameraInstance.setPreviewCallbackWithBuffer(null);
            mCameraInstance.stopPreview();
            mCameraInstance.release();
            mCameraInstance = null;
            Log.d("BFD_CameraLoader", " release camera --");
        }
    }

    //callback for 
    private static void onProcessingDone(Object ref, int currentFrameId) {
        Log.d("VSMainActivity", " BFD_onTransformCalcDone ++ ");

        WeakReference<BFDMainActivity> vsWeakRef = (WeakReference<BFDMainActivity>) ref;
        BFDMainActivity vsRef = vsWeakRef.get();

        if (vsRef == null) {
            return;
        }

        Log.d("VSMainActivity", " BFD_onProcessingDone to put id to mProcessingDoneQueue: " + currentFrameId);
        try {
            long startW = SystemClock.elapsedRealtime();
            mProcessingDoneQueue.put(currentFrameId);
            long endW = SystemClock.elapsedRealtime() - startW;
            Log.d("VSMainActivity", " BFD_onTransformCalcDone mutex wait put " + endW + " ms: " + currentFrameId);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        Log.d("VSMainActivity", "BFD_onProcessingDone -- ");
    }

    class RenderThread implements Runnable {
        public void run() {
            Log.d("BFD_RenderThread", " run ++");
            while (!mThreadExit) {
                try {
                    Log.d("BFD_RenderThread", " to get Id from TransformDoneQueue of size: " + mProcessingDoneQueue.size());
                    long startW = SystemClock.elapsedRealtime();
                    int idx = mProcessingDoneQueue.take();
                    long endW = SystemClock.elapsedRealtime() - startW;
                    Log.d("BFD_RenderThread", " mutex wait get " + endW + " ms: " + idx);
                    if (-1 != idx) {
                        mRenderer.addToQueue(idx);
                        mGPUImage.requestRender();
                        mActivityView.SetData(numFaceDet, mFaceScores, mFaceBoundingBoxes, mFaceIdTracked);
                        int[] layoutArray = mTouchEventHelper.getLayoutArray();
//                        mFaceBoundingBoxes[0] = 50 ;
//                        mFaceBoundingBoxes[1] = 50 ;
//                        mFaceBoundingBoxes[2] = 50 ;
//                        mFaceBoundingBoxes[3] = 50 ;
                        mFaceBoundingBoxes[4] = layoutArray[0];
                        mFaceBoundingBoxes[5] = layoutArray[1];
                        mFaceBoundingBoxes[6] = layoutArray[2];
                        mFaceBoundingBoxes[7] = layoutArray[3];

                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mActivityView.invalidate();
                            }
                        });
                    } else {
                        Log.d("BFD_RenderThread", " got exit element");
                        break;
                    }

                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }

            }
            Log.d("BFD_RenderThread", " run --");
        }
    }

    private void calculateViewPosition(float x, float y, float ex, float ey) {
    	if(isInit == 1){
    		RelativeLayout.LayoutParams rl = (LayoutParams) systemFrame.getLayoutParams();
            rl.leftMargin = (int) Math.min(x, ex);
            rl.topMargin = (int) Math.min(y, ey);
            rl.width = Math.abs((int) (x - ex));
            rl.height = Math.abs((int) (y - ey));

            String sendString = "A"+rl.leftMargin + "\rB" + rl.topMargin + "\rC" + rl.width + "\rD" + rl.height + "\n";
        	if(isConnected && startSend){
    			mBTService.write(sendString.getBytes());
    		}
            systemFrame.setLayoutParams(rl);
    	}
        
    }

    class BFDFrameProcessing implements Runnable {
        public void run() {
            while (!mThreadExit) {
                try {
                    long startW = SystemClock.elapsedRealtime();
                    int currentFrameId = mBuffFilledQueue.take();
                    if (-1 == currentFrameId)
                        break;
                    long endW = SystemClock.elapsedRealtime() - startW;

                    long startR = SystemClock.elapsedRealtime();


                    if (true) {    //sync call
                        long startT = SystemClock.elapsedRealtime();
                        if (null == mFrameBuffArrY[currentFrameId]) {
                            break;
                        }


                        babyFaceDetYUV(mFrameBuffArrY[currentFrameId],  mFrameWidth, mFrameHeight, 
                        		mFaceBoundingBoxes, mFaceScores, mFaceIdTracked, numFaceDet,isInit);

                        runOnUiThread(new Runnable() {

                            @Override
                            public void run() {
                                calculateViewPosition(mFaceBoundingBoxes[0], mFaceBoundingBoxes[1], mFaceBoundingBoxes[2], mFaceBoundingBoxes[3]);
                            }
                        });


                        long endT = SystemClock.elapsedRealtime() - startT;


                        long startW2 = SystemClock.elapsedRealtime();
                        mProcessingDoneQueue.put(currentFrameId);
                        long endW2 = SystemClock.elapsedRealtime() - startW2;
                        long endR = SystemClock.elapsedRealtime() - startR;
                    }
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }

    }

    static {
        System.loadLibrary("BabyFaceDetDemo");
    }

    private static native void babyFaceDetYUV(byte[] yuv,  int width, int height, float[] boundingBoxes, byte[] scores, int[] faceIds, int[] numFaceDet,int isInit);

//    private static native void initFaceDet(int maxSize, int minSize, int numMaxFace, int searchStep);
//
//    private static native void deInitFaceDet();
//
//    private static native void flipVerticalYUV(byte[] yuvIn, int width, int height, byte[] yOut, byte[] uvOut);

}
