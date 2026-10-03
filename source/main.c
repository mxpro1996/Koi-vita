#include "utils/init.h"
#include "utils/glutil.h"

#include <psp2/kernel/threadmgr.h>

#include <falso_jni/FalsoJNI.h>
#include <so_util/so_util.h>

#ifndef NDK_PORT
#include "reimpl/controls.h"
#else
#include <falso_ndk/FalsoNDK.h>
#endif

#include "reimpl/asset_manager.h"
//#include <freetype2/ftbuild.h>

int _newlib_heap_size_user = 256 * 1024 * 1024;

#ifdef USE_SCELIBC_IO
int sceLibcHeapSize = 4 * 1024 * 1024;
#endif



so_module so_mod;
typedef (*nativeTouches_func_t) (JNIEnv * env, jobject thiz, jint id, jfloat x, jfloat y);
nativeTouches_func_t nativeTouchesBegin,nativeTouchesEnd;

int main() {
    soloader_init_all();

    int (*JNI_OnLoad)(void *jvm) = (void *)so_symbol(&so_mod, "JNI_OnLoad");
    JNI_OnLoad(&jvm);

    gl_init();

#ifndef NDK_PORT
    // ... do some initialization
    void* (*nativeSetApkPath)(void*,void*,char*)  = so_symbol(&so_mod, "Java_org_cocos2dx_lib_Cocos2dxHelper_nativeSetApkPath");
    if (nativeSetApkPath == NULL) {
        sceClibPrintf("Error: Could not find nativeSetApkPath symbol!\n");
        return;
    }
    nativeSetApkPath(NULL,NULL,jni->NewStringUTF(&jni, DATA_PATH "asset.apk"));

    AAssetManager *assetManager =  AAssetManager_create();
    void (*nativeSetContext) (JNIEnv*  env, jobject thiz, jobject context, jobject assetManager) = so_symbol(&so_mod, "Java_org_cocos2dx_lib_Cocos2dxHelper_nativeSetContext");
    if (nativeSetContext == NULL) {
        sceClibPrintf("Error: Could not find nativeSetContext symbol!\n");
        return;
    }    
    jstring activityinstance = jni->NewStringUTF(&jni,"activityStub"); 
    nativeSetContext(&jni,NULL,activityinstance,assetManager);

    const int width=960, height=544;
    void* (*nativeInit)(void*,void*,int,int)  = so_symbol(&so_mod, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit");
    if (nativeInit == NULL) {
        sceClibPrintf("Error: Could not find nativeInit symbol!\n");
        return;
    }
    nativeInit(NULL, NULL, width, height);

    void* (*nativeRender)(void*)  = so_symbol(&so_mod, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeRender");
    if (nativeRender == NULL) {
        sceClibPrintf("Error: Could not find nativeRender symbol!\n");
        return;
    }

    nativeTouchesBegin = so_symbol(&so_mod, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesBegin");
    if (nativeTouchesBegin == NULL) {
        sceClibPrintf("Error: Could not find nativeTouchesBegin symbol!\n");
        return;
    }
    
    nativeTouchesEnd  = so_symbol(&so_mod, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesEnd");
    if (nativeTouchesEnd == NULL) {
        sceClibPrintf("Error: Could not find nativeTouchesEnd symbol!\n");
        return;
    }
    


    while (1) {
        // ... render call
	    nativeRender(NULL);
        controls_poll();
        gl_swap();
    }
#else
    // Build a fake ANativeActivity that the game's onCreate will receive
    ANativeActivity *activity = malloc(sizeof(ANativeActivity));
    activity->callbacks = malloc(sizeof(ANativeActivityCallbacks));
    activity->env = &jni; // from FalsoJNI
    activity->vm = &jvm;  // from FalsoJNI
    activity->clazz = (jclass)0x42424242;
    activity->internalDataPath = DATA_PATH "assets/";
    activity->externalDataPath = DATA_PATH "assets/";
    activity->sdkVersion = 14;
    activity->instance = NULL;

    // Drive the activity lifecycle
    int (*ANativeActivity_onCreate)(ANativeActivity *, void *, size_t) =
        (void *)so_symbol(&so_mod, "ANativeActivity_onCreate");
    ANativeActivity_onCreate(activity, NULL, 0);

    activity->callbacks->onStart(activity);
    activity->callbacks->onResume(activity);

    // Wire up input and the native window
    AInputQueue *aInputQueue = AInputQueue_create();
    activity->callbacks->onInputQueueCreated(activity, aInputQueue);

    ANativeWindow *aNativeWindow = ANativeWindow_create();
    activity->callbacks->onNativeWindowCreated(activity, aNativeWindow);

    activity->callbacks->onWindowFocusChanged(activity, 1);
#endif

    sceKernelExitDeleteThread(0);
}

#ifndef NDK_PORT
void controls_handler_key(int32_t keycode, ControlsAction action) {
    // Call into the .so here
}

void controls_handler_touch(int32_t id, float x, float y, ControlsAction action) {
    // Call into the .so here
    switch(action){
        case CONTROLS_ACTION_UP:{
            nativeTouchesEnd(NULL,NULL,id,x,y);
        }break;

        case CONTROLS_ACTION_DOWN:{
            nativeTouchesBegin(NULL,NULL,id,x,y);
        }break;

        case CONTROLS_ACTION_MOVE:{
            //sceClibPrintf("Not handle, move action\n");
        }break;
    }
}

void controls_handler_analog(ControlsStickId which, float x, float y, ControlsAction action) {
    // Call into the .so here
}
#endif
