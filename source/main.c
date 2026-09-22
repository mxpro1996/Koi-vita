#include "utils/init.h"
#include "utils/glutil.h"

#include <psp2/kernel/threadmgr.h>

#include <falso_jni/FalsoJNI.h>
#include <falso_jni/FalsoJNI_ImplBridge.h>

#include <so_util/so_util.h>

#ifndef NDK_PORT
#include "reimpl/controls.h"
#else
#include <falso_ndk/FalsoNDK.h>
#endif

int _newlib_heap_size_user = 256 * 1024 * 1024;

#ifdef USE_SCELIBC_IO
int sceLibcHeapSize = 4 * 1024 * 1024;
#endif

so_module so_mod;
so_module so_mod_libcocosden;
so_module so_mod_libcocos2d;
so_module so_mod_libgame_logic;

typedef (*nativeTouches_func_t) (JNIEnv * env, jobject thiz, jint id, jfloat x, jfloat y);
nativeTouches_func_t nativeTouchesBegin,nativeTouchesEnd;
void (*nativeTouchesMove)(JNIEnv *jni, jobject thiz, jint *ids, jfloat *xs, jfloat *ys);
void (*nativeInitBitmapDC)(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels);
extern void bm_init(void);
JavaDynArray *touch_ids, *touch_xs, *touch_ys;



#include <psp2/sysmodule.h>
#include <psp2/sqlite.h>
#include <stdlib.h>
void init_vita_sqlite() {
    sceSysmoduleLoadModule(SCE_SYSMODULE_SQLITE);

    SceSqliteMallocMethods malloc_methods;
    malloc_methods.xMalloc  = (void* (*)(int))malloc;
    malloc_methods.xRealloc = realloc;
    malloc_methods.xFree    = free;
    
    sceSqliteConfigMallocMethods(&malloc_methods);
}


int main() {
    soloader_init_all();

    int (*JNI_OnLoad)(void *jvm) = (void *)so_symbol(&so_mod_libcocos2d, "JNI_OnLoad");
    JNI_OnLoad(&jvm);

    JNI_OnLoad = (void *)so_symbol(&so_mod_libcocosden, "JNI_OnLoad");
    JNI_OnLoad(&jvm);    

    JNI_OnLoad = (void *)so_symbol(&so_mod_libgame_logic, "JNI_OnLoad");
    JNI_OnLoad(&jvm);

    gl_init();

#ifndef NDK_PORT
    // ... do some initialization
    void* (*nativeSetPaths)(void*,void*,char*)  = so_symbol(&so_mod_libcocos2d, "Java_org_cocos2dx_lib_Cocos2dxActivity_nativeSetPaths");
    const int width=960, height=544;
    void* (*nativeInit)(void*,void*,int,int)  = so_symbol(&so_mod, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit");
    void* (*nativeRender)(void*)  = so_symbol(&so_mod_libcocos2d, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeRender");
    nativeTouchesBegin = so_symbol(&so_mod_libcocos2d, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesBegin");
    nativeTouchesEnd  = so_symbol(&so_mod_libcocos2d, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesEnd");
    nativeTouchesMove  = so_symbol(&so_mod_libcocos2d, "Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesMove");
    nativeInitBitmapDC  = so_symbol(&so_mod_libcocos2d, "Java_org_cocos2dx_lib_Cocos2dxBitmap_nativeInitBitmapDC");

    touch_ids = jda_alloc(1, FIELD_TYPE_INT);
    touch_xs = jda_alloc(1, FIELD_TYPE_FLOAT);
    touch_ys = jda_alloc(1, FIELD_TYPE_FLOAT);

    init_vita_sqlite();
    bm_init();
    nativeSetPaths(&jni,NULL,jni->NewStringUTF(&jni, DATA_PATH "asset.apk"));
    nativeInit(NULL, NULL, width, height);


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
    id%=5;

    ((int *)touch_ids->array)[0] = id; // we use 0-7 for simulated touch events from buttons
    ((float *)touch_xs->array)[0] = x;
    ((float *)touch_ys->array)[0] = y;
    switch(action){
        case CONTROLS_ACTION_UP:{
            // sceClibPrintf("Touched: %d - (%.f,%.f)\n",id,x,y);
            nativeTouchesEnd(&jni,NULL,id,x,y);
        }break;

        case CONTROLS_ACTION_DOWN:{
            nativeTouchesBegin(&jni,NULL,id,x,y);
        }break;

        case CONTROLS_ACTION_MOVE:{
            nativeTouchesMove(&jni,NULL,touch_ids, touch_xs, touch_ys);
            //sceClibPrintf("Not handle, move action\n");
        }break;
    }
}

void controls_handler_analog(ControlsStickId which, float x, float y, ControlsAction action) {
    // Call into the .so here
}
#endif
