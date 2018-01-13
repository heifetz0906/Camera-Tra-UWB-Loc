#ifndef qcv_offbrobustTracker_motionAnalysis_h_
#define qcv_offbrobustTracker_motionAnalysis_h_

#include <time.h> 
#include <string>
#include <iostream> // cout
#include <fstream>  // ifstream
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <fastcv.h> //FastCV Header file

namespace qcv
{

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#define N_LOCALVEC 8 //12
#define MAX_LOCALVEC (N_LOCALVEC*N_LOCALVEC)
#define MAX_GLOBALVEC 25
#define MV_INVALID -1000



//Central Weight Matrix 
#if N_LOCALVEC == 12
static float32_t c_weight[144] = {         
                                     0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f,
                                     0.25f, 0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.7f,  0.7f,  0.7f,  0.7f,  0.7f,  0.7f,  0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.7f,  0.85f, 0.85f, 0.85f, 0.85f, 0.7f,  0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.7f,  0.85f, 1.0f,  1.0f,  0.85f, 0.7f,  0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.7f,  0.85f, 1.0f,  1.0f,  0.85f, 0.7f,  0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.7f,  0.85f, 0.85f, 0.85f, 0.85f, 0.7f,  0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.7f,  0.7f,  0.7f,  0.7f,  0.7f,  0.7f,  0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.55f, 0.4f,  0.25f,
                                     0.25f, 0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.4f,  0.25f,
                                     0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f
};
#endif

#if N_LOCALVEC == 8
static float32_t c_weight[64] = { 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f,
0.25f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.25f,
0.25f, 0.5f, 0.75f, 0.75f, 0.75f, 0.75f, 0.5f, 0.25f,
0.25f, 0.5f, 0.75f, 0.1f, 0.1f, 0.75f, 0.5f, 0.25f,
0.25f, 0.5f, 0.75f, 0.1f, 0.1f, 0.75f, 0.5f, 0.25f,
0.25f, 0.5f, 0.75f, 0.75f, 0.75f, 0.75f, 0.5f, 0.25f,
0.25f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.25f,
0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f };
#endif


//Local Functions

class MotionAnalyzer
{

private :

    //Variables

    //Orientation based
    float32_t magG[MAX_GLOBALVEC], magL[MAX_LOCALVEC];
    float32_t orientationG[MAX_GLOBALVEC], orientationL[MAX_LOCALVEC];
    float32_t magGStats[2], magLStats[2], orientationGStats[2], orientationLStats[2];

    //X-Y based
    float32_t XG[MAX_GLOBALVEC], XL[MAX_LOCALVEC];
    float32_t YG[MAX_GLOBALVEC], YL[MAX_LOCALVEC];
    float32_t XGStats[2], XLStats[2], YGStats[2], YLStats[2];

    //Functions


public :
   
    //Variables
    bool isGlobalValid, isLocalValid; //Check if Global Motion, Local Motion are consistent
    float32_t glb_median_mv[2];         //Motion Vector corresponding to Median of Global Motion  
    float32_t fg_label[MAX_LOCALVEC];
    float32_t weight_hist[MAX_LOCALVEC];
    bool isTracked[MAX_LOCALVEC];
    //Functions
    MotionAnalyzer();
    virtual ~MotionAnalyzer();

    //General Functions
    void smoothMV(float32_t* mv_in, uint32_t nTrackedPoints, int32_t* trueIdx);
    void smoothPoints(float32_t* vecPoints, uint32_t nTrackedPoints);
    void smoothMV(float32_t* mv_in, uint32_t nTrackedPoints);
    //Orientation based
   //Check if global motion is consistent. If so, find the median.
   void evaluateGlobalMotion(float32_t* mv_global, uint32_t nVectors); 

   //Check how similar local motion is to global motion
   void evaluateLocalMotion(float32_t* mv_local, uint32_t nLocVectors);

   //Joint Segmentation using both magnitude and orientation of motion vectors
   void jointSegmentation(uint32_t nVectors);

   //Position based
   //Check if global motion is consistent. If so, find the median.
   void evaluateGlobalMotionXY(float32_t* mv_global, uint32_t nVectors);

   //Check how similar local motion is to global motion
   void evaluateLocalMotionXY(float32_t* mv_local, uint32_t nLocVectors);

   //Joint Segmentation using both magnitude and orientation of motion vectors
   void jointSegmentationXY(float32_t* mv_local, uint32_t nVectors,int32_t* indices);
   void jointSegmentationXY(float32_t* mv_local, uint32_t nVectors);

   //Normalize Weights
   void normalizeWeights();

   //Update Weights
   void updateWeights();

   //Reset data
   void resetData();

   //Reset History
   void resetHistory();

};


}//namespace qcv
#endif

