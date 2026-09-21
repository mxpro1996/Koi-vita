#include <falso_jni/FalsoJNI.h>
#include <falso_jni/FalsoJNI_Impl.h>
#include <falso_jni/FalsoJNI_Logger.h>

jstring getDeviceModel(jmethodID id, va_list args) {
    const char * phoneModel = "PCH-2000";
    
    return jni->NewStringUTF(&jni, phoneModel);
}

jstring getCocos2dxWritablePath(jmethodID id, va_list args) {
	const char dpath[] = "ux0:data/narutoSenki";
    return jni->NewStringUTF(&jni, dpath);
}

void setAnimationInterval(jmethodID id, va_list args) {
    return;
}

/*
 * JNI Methods
*/

NameToMethodID nameToMethodId[] = {
    { 100, "getDeviceModel", METHOD_TYPE_OBJECT },
	{ 101, "setAnimationInterval", METHOD_TYPE_VOID },
	{ 102, "getCocos2dxWritablePath", METHOD_TYPE_OBJECT },
};

MethodsBoolean methodsBoolean[] = {};
MethodsByte methodsByte[] = {};
MethodsChar methodsChar[] = {};
MethodsDouble methodsDouble[] = {};
MethodsFloat methodsFloat[] = {};
MethodsInt methodsInt[] = {};
MethodsLong methodsLong[] = {};
MethodsObject methodsObject[] = {
	{ 100, getDeviceModel },
	{ 102, getCocos2dxWritablePath },
};
MethodsShort methodsShort[] = {};
MethodsVoid methodsVoid[] = {
	{ 101, setAnimationInterval},
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
