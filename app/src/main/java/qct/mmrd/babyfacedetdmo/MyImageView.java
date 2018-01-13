package qct.mmrd.babyfacedetdmo;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.widget.ImageView;

public class MyImageView extends ImageView{
	private Paint mPaint;
	private int[] mNumDetFaces;
	private float mLength;
	private float mAngle;
	private float[] mBoundingBoxes;
	private int[] mIds;
	private int MAX_NUM_FACES = qct.mmrd.babyfacedetdmo.BFDMainActivity.MAX_NUM_FACES;
	
	public MyImageView(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		mPaint = new Paint();
		mPaint.setStrokeWidth(5);
		mPaint.setTextSize(30); 
		mNumDetFaces = new int[1];
		
		mBoundingBoxes = new float[8*MAX_NUM_FACES];
		mIds = new int[MAX_NUM_FACES];
	}
	public void SetData(int[] numDetFaces, byte[] scores, float[] boundingBoxes, int[] faceIdTracked)
	{
		mLength = (float)boundingBoxes[4];
		mAngle = (float)boundingBoxes[5];
			mBoundingBoxes[0]= boundingBoxes[0] *getWidth()/640;
		    mBoundingBoxes[2]= boundingBoxes[2] *getWidth()/640;
		    mBoundingBoxes[4]= boundingBoxes[4] *getWidth()/640;
		    mBoundingBoxes[6]= boundingBoxes[6] *getWidth()/640;
		    mBoundingBoxes[1]= boundingBoxes[1] * getHeight()/480;
		    mBoundingBoxes[3]= boundingBoxes[3] * getHeight()/480;
		    mBoundingBoxes[5]= boundingBoxes[5] * getHeight()/480;
		    mBoundingBoxes[7]= boundingBoxes[7] * getHeight()/480;
		
		
	}
	
	protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        // And draw your line.
        // (Be sure to have set the values/fields in linePaint earlier so that you draw the correct line/type/size/etc).
        //canvas.drawLine(0, getHeight()/2, getWidth(), getHeight()/2, mPaint);
           int score = 0;
           
            int color0=score;
            int color1=255-score;
            int color=0xff000000;
            color =color+ (color0<<16) +(color1<<8);
        	/*if(score>128)
        		mPaint.setColor(Color.RED);
        	else
            	mPaint.setColor(Color.GREEN);
        	*/
            mPaint.setColor(color);
            
      
            if (mBoundingBoxes[2]>=getWidth()-10)
            	mBoundingBoxes[2]=getWidth()-11;
            if (mBoundingBoxes[3]>=getHeight()-10)
            	mBoundingBoxes[3]=getHeight()-11;
            	
            	 
            
            
            String.format("startx starty endx endy %f %f %f %f ", mBoundingBoxes[0],mBoundingBoxes[1],mBoundingBoxes[2],mBoundingBoxes[3]);
        	canvas.drawLine(mBoundingBoxes[0], mBoundingBoxes[1], mBoundingBoxes[2], mBoundingBoxes[3], mPaint);
        	
        	
        	int headlen = 20;   // length of head in pixels
             double angle = Math.atan2(mBoundingBoxes[3]-mBoundingBoxes[1],mBoundingBoxes[2]-mBoundingBoxes[0]);
            float arrow_x = (float) (mBoundingBoxes[2]-headlen*Math.cos(angle-Math.PI/6));
            float arrow_y = (float) (mBoundingBoxes[3]-headlen*Math.sin(angle-Math.PI/6));
            
            if (mBoundingBoxes[2]-mBoundingBoxes[0]!=0 || mBoundingBoxes[1]-mBoundingBoxes[3]!=0)
            	
            	
            canvas.drawLine(arrow_x, arrow_y, mBoundingBoxes[2], mBoundingBoxes[3], mPaint);
            
            
             arrow_x = (float) (mBoundingBoxes[2]-headlen*Math.cos(angle+Math.PI/6));
             arrow_y = (float) (mBoundingBoxes[3]-headlen*Math.sin(angle+Math.PI/6));
             
             if (mBoundingBoxes[2]-mBoundingBoxes[0]!=0 || mBoundingBoxes[1]-mBoundingBoxes[3]!=0)
             canvas.drawLine(arrow_x, arrow_y, mBoundingBoxes[2], mBoundingBoxes[3], mPaint);
             
            
        	//canvas.drawText("ID: "+String.valueOf(id)+"Score: "+String.valueOf(score), mBoundingBoxes[8*i], mBoundingBoxes[8*i+1],mPaint);
        	canvas.drawText("x: "+String.valueOf((int)(mBoundingBoxes[2]-mBoundingBoxes[0])), mBoundingBoxes[0]+30, mBoundingBoxes[1],mPaint);
        	canvas.drawText("y: "+String.valueOf((int)(mBoundingBoxes[3]-mBoundingBoxes[1])), mBoundingBoxes[0]+30, mBoundingBoxes[1]+30,mPaint);
       

    }
	

}
