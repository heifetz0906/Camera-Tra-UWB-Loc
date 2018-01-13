LOCAL_PATH := $(call my-dir)
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE := libeOkaoCo 
LOCAL_SRC_FILES := lib/libeOkaoCo.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE := libeOkaoDt 
LOCAL_SRC_FILES := lib/libeOkaoDt.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE := libeOkaoSm 
LOCAL_SRC_FILES := lib/libeOkaoSm.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libeOkaoPt
LOCAL_SRC_FILES := lib/libeOkaoPt.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libeOkaoPc
LOCAL_SRC_FILES := lib/libeOkaoPc.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libeOkaoFr
LOCAL_SRC_FILES := lib/libeOkaoFr.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libeOkaoGb
LOCAL_SRC_FILES := lib/libeOkaoGb.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 
##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libeOkaoPd
LOCAL_SRC_FILES := lib/libeOkaoPd.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
 

##############################################################################
include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libeOkaoCt
LOCAL_SRC_FILES := lib/libeOkaoCt.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
##############################################################################

include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libfastcv
LOCAL_SRC_FILES := lib/libfastcv.a 
 
include $(PREBUILT_STATIC_LIBRARY) 
##############################################################################

include $(CLEAR_VARS) 
 
LOCAL_MODULE :=  libtracker
LOCAL_SRC_FILES := lib/libtracker.so
 
include $(PREBUILT_SHARED_LIBRARY) 
##############################################################################


include $(CLEAR_VARS)

LOCAL_MODULE    := BabyFaceDetDemo
LOCAL_SRC_FILES := BabyFaceDetDemo.cpp

LOCAL_CPP_FEATURES += exceptions
LOCAL_CFLAGS += -std=c++11

LOCAL_LDLIBS +=  -llog -ldl

LOCAL_STATIC_LIBRARIES += libeOkaoDt \
				libeOkaoPt \
				libeOkaoSm \
				libeOkaoCt \
				libeOkaoPc \
				libeOkaoFr \
				libeOkaoGb \
				libeOkaoPd \
				libeOkaoCo \
				libfastcv
				


LOCAL_SHARED_LIBRARIES += libtracker

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc/

include $(BUILD_SHARED_LIBRARY)
