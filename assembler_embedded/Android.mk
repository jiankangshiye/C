LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := assignment.c
LOCAL_MODULE := assignment
LOCAL_STATIC_LIBRARIES := libc
LOCAL_MODULE_PATH:= $(LOCAL_PATH)
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_C_FLAGS := -Wall

include $(BUILD_EXECUTABLE)

