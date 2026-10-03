#include <falso_jni/FalsoJNI.h>
#include <falso_jni/FalsoJNI_Impl.h>
#include <falso_jni/FalsoJNI_Logger.h>

#include "audio.h"


jstring getDeviceModel(jmethodID id, va_list args) {
    const char * phoneModel = "PCH-2000";
    
    return jni->NewStringUTF(&jni, phoneModel);
}

void setAnimationInterval(jmethodID id, va_list args)
{
	jdouble interval = va_arg(args, jdouble);
	fjni_logv_info("[FalsoJNI] setAnimationInterval(%f) called", (float)interval);
}

jstring getCocos2dxWritablePath(jmethodID id, va_list args) {
	const char dpath[] = "ux0:data/narutoSenki";
    return jni->NewStringUTF(&jni,"");
}

/*
	if (!JniHelper::getMethodInfo_DefaultClassLoader(_m,
														"java/lang/ClassLoader",
														"loadClass",
														"(Ljava/lang/String;)Ljava/lang/Class;")) {
		return false;
	}
*/
jclass loadClass(jmethodID id, va_list args){
	// class stubReturn
	const char *className = jni->GetStringChars(&jni, va_arg(args, jstring), NULL);
	return jni->FindClass(&jni, className);
}

/*
	if (!JniHelper::getMethodInfo_DefaultClassLoader(_getclassloaderMethod,
														"android/content/Context",
														"getClassLoader",
														"()Ljava/lang/ClassLoader;")) {
		return false;
	}

*/
jobject getClassLoader(jmethodID id, va_list args){
	// classloader ret
	return jni->NewStringUTF(&jni, "classLoaderStub");
}



/*
 * JNI Methods
*/

NameToMethodID nameToMethodId[] = {
    { 96, "getClassLoader", METHOD_TYPE_OBJECT },
    { 97, "loadClass", METHOD_TYPE_OBJECT },
    { 98, "getDeviceModel", METHOD_TYPE_OBJECT },
	{ 99, "getCocos2dxWritablePath", METHOD_TYPE_OBJECT },
	{100, "setAnimationInterval", METHOD_TYPE_VOID},
	// sound shit
	{108, "preloadEffect", METHOD_TYPE_VOID},
	{122, "unloadEffect", METHOD_TYPE_VOID},
	{123, "playEffect", METHOD_TYPE_INT},
	{124, "setEffectVolume", METHOD_TYPE_VOID},
	{125, "setEffectRate", METHOD_TYPE_VOID},
	{126, "stopEffect", METHOD_TYPE_VOID},
	{127, "pauseEffect", METHOD_TYPE_VOID},
	{128, "resumeEffect", METHOD_TYPE_VOID},
	{129, "pauseAllEffects", METHOD_TYPE_VOID},
	{130, "resumeAllEffects", METHOD_TYPE_VOID},
	{131, "stopAllEffects", METHOD_TYPE_VOID},
	{132, "getEffectsVolume", METHOD_TYPE_FLOAT},
	{133, "setEffectsVolume", METHOD_TYPE_VOID},
	// music shit
	{200, "preloadBackgroundMusic", METHOD_TYPE_VOID},
	{201, "playBackgroundMusic", METHOD_TYPE_VOID},
	{202, "stopBackgroundMusic", METHOD_TYPE_VOID},
	{203, "pauseBackgroundMusic", METHOD_TYPE_VOID},
	{204, "resumeBackgroundMusic", METHOD_TYPE_VOID},
	{205, "rewindBackgroundMusic", METHOD_TYPE_VOID},
	{206, "isBackgroundMusicPlaying", METHOD_TYPE_BOOLEAN},
	{207, "endBackgroundMusic", METHOD_TYPE_VOID},
	{208, "getBackgroundVolume", METHOD_TYPE_FLOAT},
	{209, "setBackgroundMusicVolume", METHOD_TYPE_VOID},

};

MethodsBoolean methodsBoolean[] = {
	{206, isBackgroundMusicPlaying},
};
MethodsByte methodsByte[] = {};
MethodsChar methodsChar[] = {};
MethodsDouble methodsDouble[] = {};
MethodsFloat methodsFloat[] = {
	{132, getEffectsVolume},
	{208, getBackgroundVolume},
};
MethodsInt methodsInt[] = {
	{123, playEffect},
};

MethodsLong methodsLong[] = {};
MethodsObject methodsObject[] = {
	{ 96, getClassLoader },
    { 97, loadClass },
	{ 98, getDeviceModel },
	{ 99, getCocos2dxWritablePath },
};
MethodsShort methodsShort[] = {};
MethodsVoid methodsVoid[] = {
	{100, setAnimationInterval},
	{108, preloadEffect},
	{122, unloadEffect},
	{124, setEffectVolume},
	{125, setEffectRate},
	{126, stopEffect},
	{127, pauseEffect},
	{128, resumeEffect},
	{129, pauseAllEffects},
	{130, resumeAllEffects},
	{131, stopAllEffects},
	{133, setEffectsVolume},
	{200, preloadBackgroundMusic},
	{201, playBackgroundMusic},
	{202, stopBackgroundMusic},
	{203, pauseBackgroundMusic},
	{204, resumeBackgroundMusic},
	{205, rewindBackgroundMusic},
	{207, endBackgroundMusic},
	{209, setBackgroundVolume},
};

/*
 * JNI Fields
*/

// System-wide constant that applications sometimes request
// https://developer.android.com/reference/android/content/Context.html#WINDOW_SERVICE
char WINDOW_SERVICE[] = "window";

// System-wide constant that's often used to determine Android version
// https://developer.android.com/reference/android/os/Build.VERSION.html#SDK_INT
// Possible values: https://developer.android.com/reference/android/os/Build.VERSION_CODES
const int SDK_INT = 19; // Android 4.4 / KitKat

NameToFieldID nameToFieldId[] = {
		{ 0, "WINDOW_SERVICE", FIELD_TYPE_OBJECT }, 
		{ 1, "SDK_INT", FIELD_TYPE_INT },
};

FieldsBoolean fieldsBoolean[] = {};
FieldsByte fieldsByte[] = {};
FieldsChar fieldsChar[] = {};
FieldsDouble fieldsDouble[] = {};
FieldsFloat fieldsFloat[] = {};
FieldsInt fieldsInt[] = {
		{ 1, SDK_INT },
};
FieldsObject fieldsObject[] = {
		{ 0, WINDOW_SERVICE },
};
FieldsLong fieldsLong[] = {};
FieldsShort fieldsShort[] = {};

__FALSOJNI_IMPL_CONTAINER_SIZES
