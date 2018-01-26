package qct.mmrd.babyfacedetdmo;

import android.util.Log;

import static android.R.attr.x;
import static java.lang.Math.abs;

/**
 * Created by 59732 on 2018/1/18.
 */

public class FollowOBJ {

    private final String TAG = FollowOBJ.this.getClass().getSimpleName();

    //以下是設定目標值
    private int top;//距离左边边沿的位置
    private int left;//距离上边沿的位置
    private int width;//方框的宽度
    private int height;//方框的高度
    //以下是本次误差    拟采用增量式 pid 控制
    private int top_error=0;//距离左边边沿的位置 誤差
    private int left_error=0;//距离上边沿的位置 誤差
    private int width_error=0;//方框的宽度 誤差
    private int height_error=0;//方框的高度 誤差
    //以下是上次误差    拟采用增量式 pid 控制
    private int top_last_error=0;//距离左边边沿的位置 誤差
    private int left_last_error=0;//距离上边沿的位置 誤差
    private int width_last_error=0;//方框的宽度 誤差
    private int height_last_error=0;//方框的高度 誤差
    //以下是误差的误差   拟采用增量式 pid 控制
    private int top_e_error=0;
    private int left_e_error=0;
    private int width_e_error=0;
    private int height_e_error=0;

    private double xP,xI,xD,yP,yI,yD,zP,zI,zD;//设定三个参数
    //丢失目标标志位
    private boolean loose=false;
    //目前只用y和z轴，也就是前进后退以及转向，不使用横向移动
    public FollowOBJ(int x, int y, int width, int height) {
        this.top = x;
        this.left = y;
        this.width = width;
        this.height = height;
        this.yP=0.5;
        this.yD=0;
        this.zP=0.2;
        this.zD=0;
    }//构造构造函数，用于设定目标值

//    public String calculate()
//    {
//        StringBuilder send=new StringBuilder();
//        if(now_x==0&&now_y==0&&now_width==0&&now_height==0)
//        {
//            if(loose==false) {
//                loose = true;
//            send.append("K");
//            }
//            send.append("G");//如果丢失目标，就顺时针旋转寻找目标
//            return send.toString();
//        }else {
//            if(loose==true)
//            {
//                loose=false;
//                send.append("J");
//            }
//            if((abs(x_error)<20)&&(abs(y_error)<20)&&(abs(w_error)<20)&&(abs(h_error)<20))
//            {
//                send.append("Z");//差距不大时停止
//                return send.toString();
//            }else
//            {
//               if(x_error<-20&&y_error<-20)//x误差小于-20 y误差小于-20，方块位于左上方，小车应该向左后方移动。
//               {
//                   send.append("F");
//                   return send.toString();
//               }else if(x_error<-20&&y_error>20)//x误差小于-20 y误差大于20，方块位于左下方，小车应该向左前方移动。
//               {
//                   send.append("H");
//                   return send.toString();
//               }else if (x_error>20&&y_error<-20)//x误差大于20 y误差小于-20，方块位于右上方，小车应该向右后方移动。
//               {
//                   send.append("D");
//                   return send.toString();
//               }else if (x_error>20&&y_error>20)//x误差大于20 y误差大于20，方块位于右下方，小车应该向右前方移动。
//               {
//                   send.append("B");
//                   return send.toString();
//               }
//            }
//        }
//        return send.toString();
//    }

    public int pidcalc(byte[] input)
    {
        int y,z;
        if(width_error>0) {  //跟随前进的时候速度加快一点
            y = (int) ((yP+0.2)* width_error + yD * width_e_error);     //width_error大于0的时候是说明，目标太远。需要前进
        }else
        {//后退时正常速度
            y = (int) (yP* width_error + yD * width_e_error);
        }
        z=(int )(zP*left_error+zD*left_e_error);        //left_error大于0的时候是说明，目标在右。需要左转
        if (y>40)y=40;
        if(y<-40)y=-40;
        if (z>40)z=40;
        if(z<-40)z=-40;
        Log.d(TAG,"y:"+y+"Z:"+z+"width_error:"+width_error+"left_error"+left_error);
        if(loose==true)
        {
            int2byte(input, 0, 0, 30);
            return 0;
        }
        else {
            if (abs(y) < 20 && abs(z) < 20) {
                int2byte(input, 0, y, z);
                return 0;
            } else {
                int2byte(input, 0, y, z);
            }
        }
        return  0;
    }

    public int getX() {
        return top;
    }

    public void setX(int x) {
        this.top = x;
    }

    public int getY() {
        return left;
    }

    public void setY(int y) {
        this.left = y;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public void setPoint(int now_top,int now_left,int now_width,int now_height)
    {
        this.top=now_top;
        this.left=now_left;
        this.width=now_width;
        this.height=now_height;
    }

    public void set_error(int now_top,int now_left,int now_width,int now_height)
    {
        if(now_top==0&&now_left==0&&now_width==0&&now_height==0)
        {
            loose=true;
        }else {
            //上次误差
            this.top_last_error = this.top_error;
            this.left_last_error = this.left_error;
            this.width_last_error = this.width_error;
            this.height_last_error = this.height_error;
            //本次误差
            this.top_error = now_top - this.top;
            this.left_error = this.left - now_left;    //y轴是前后方向，前是正，后是负。所以用设定值减去目前值
            this.width_error = this.width - now_width; //z轴是旋转方向，逆时针是正，顺时针是负。所以用设定值减去目前值
            this.height_error = now_height - this.height;
            //误差的误差
            this.top_e_error = this.top_last_error - this.top_error;
            this.left_e_error = this.left_last_error - this.left_error;
            this.width_e_error = this.width_last_error - this.width_error;
            this.height_e_error = this.height_last_error - this.height_error;
            loose=false;
        }
    }

    private void int2byte (byte[] in,int x,int y,int z)
    {
        if(x>=0)
        {
            in[9]&=0xFB;
            in[9]|=0x00;
        }else
        {
            in[9]&=0xFB;
            in[9]|=0x04;
            x=-x;
        }
        in[3]=(byte) ((x&0xFF00)>>8);
        in[4]=(byte)((x&0x00FF));
        if(y>=0)
        {
            in[9]&=0xFD;
            in[9]|=0x00;
        }else
        {
            in[9]&=0xFB;
            in[9]|=0x02;
            y=-y;
        }
        in[5]=(byte) ((y&0xFF00)>>8);
        in[6]=(byte)((y&0x00FF));
        if(z>=0)
        {
            in[9]&=0xFE;
            in[9]|=0x00;
        }else
        {
            in[9]&=0xFE;
            in[9]|=0x01;
            z=-z;
        }
        in[7]=(byte) ((z&0xFF00)>>8);
        in[8]=(byte)((z&0x00FF));
    }
}

