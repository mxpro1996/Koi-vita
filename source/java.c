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

extern void bm_draw(const char *str,const char *fontName,int pFontSize,int pAlignment,int pWidth,int pHeight);

static void createTextBitmap(jmethodID id, va_list args){

    jstring pString = va_arg(args,jstring);
    jstring pFontName = va_arg(args,jstring);

    const char *str = jni->GetStringUTFChars(&jni,pString,NULL);
    const char *fontName = jni->GetStringUTFChars(&jni,pFontName,NULL);
    jint pFontSize = va_arg(args,jint);
    jint pAlignment = va_arg(args,jint);
    jint pWidth = va_arg(args,jint);
    jint pHeight = va_arg(args,jint);
    
    if(str && fontName){
        sceClibPrintf("%s %s %d %d (%d %d)\n", str, fontName, pFontSize, pAlignment, pWidth, pHeight);
        bm_draw(str,fontName,pFontSize,pAlignment,pWidth,pHeight);
    }else{
        sceClibPrintf("createTextBitmap: err\n");
    }


    return;
}


#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

// Standard path for FalsoJNI ports (e.g., PS Vita ux0:data directory)
#define DB_BASE_PATH "ux0:data/detective/databases/"

static sqlite3* commonDB = NULL;
static sqlite3* resourceDB = NULL;
static sqlite3* saveDB = NULL;
static sqlite3* newSaveDB = NULL;


// ============================================================================
// Internal SQLite Helpers
// ============================================================================

static jboolean open_db(sqlite3** db, const char* filename) {
    char path[256];
    snprintf(path, sizeof(path), "%s%s", DB_BASE_PATH, filename);
    if (sqlite3_open(path, db) == SQLITE_OK) {
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

static void exec_non_return_query(sqlite3* db, jstring _query) {
    if (!db) return;
    const char* query = jni->GetStringUTFChars(&jni, _query, NULL);
    char* err_msg = NULL;
    sqlite3_exec(db, query, 0, 0, &err_msg);
    if (err_msg) {
        sqlite3_free(err_msg);
    }
    jni->ReleaseStringUTFChars(&jni, _query, query);
}

static jobject return_data_from_db(sqlite3* db, jstring _query) {
    if (!db) return NULL;
    const char* query = jni->GetStringUTFChars(&jni, _query, NULL);
    sqlite3_stmt* stmt;

    jobjectArray rowArray = NULL;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
        int cols = sqlite3_column_count(stmt);
        
        // Count rows first (SQLite doesn't provide direct row count without stepping)
        int rows = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) rows++;
        sqlite3_reset(stmt);

        // Find String class and create 2D array
        jclass stringClass = jni->FindClass(&jni, "java/lang/String");
        jclass objectArrayClass = jni->FindClass(&jni, "[Ljava/lang/String;");
        rowArray = jni->NewObjectArray(&jni, rows, objectArrayClass, NULL);

        int r = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            jobjectArray colArray = jni->NewObjectArray(&jni, cols, stringClass, NULL);
            for (int c = 0; c < cols; c++) {
                const char* val = (const char*)sqlite3_column_text(stmt, c);
                jstring jval = jni->NewStringUTF(&jni, val ? val : "");
                jni->SetObjectArrayElement(&jni, colArray, c, jval);
                jni->DeleteLocalRef(&jni, jval);
            }
            jni->SetObjectArrayElement(&jni, rowArray, r, colArray);
            jni->DeleteLocalRef(&jni, colArray);
            r++;
        }
        sqlite3_finalize(stmt);
    }

    jni->ReleaseStringUTFChars(&jni, _query, query);
    return (jobject)rowArray;
}

// ============================================================================
// Method Implementations
// ============================================================================

// --- Void Methods (Lifecycle & Context) ---
void SqliteManager_setContext(jmethodID id, va_list args) { /* No-op in native ports */ }
void SqliteManager_setActivity(jmethodID id, va_list args) { /* No-op in native ports */ }
void SqliteManager_setTimePrivate(jmethodID id, va_list args) { /* No-op */ }
void SqliteManager_moveReviewPage(jmethodID id, va_list args) { /* Stub */ }
void SqliteManager_moveBannerPage(jmethodID id, va_list args) { /* Stub */ }
void SqliteManager_playVibrator(jmethodID id, va_list args) { /* Call native vibrator if needed */ }

void SqliteManager_purchaseItem(jmethodID id, va_list args) {
    jstring _pID = va_arg(args, jstring);
    // Mock purchase logic
}

void SqliteManager_closeCommonDB(jmethodID id, va_list args) {
    if (commonDB) { sqlite3_close(commonDB); commonDB = NULL; }
}
void SqliteManager_closeResourceDB(jmethodID id, va_list args) {
    if (resourceDB) { sqlite3_close(resourceDB); resourceDB = NULL; }
}
void SqliteManager_closeSaveDB(jmethodID id, va_list args) {
    if (saveDB) { sqlite3_close(saveDB); saveDB = NULL; }
}
void SqliteManager_closeNewSaveDB(jmethodID id, va_list args) {
    if (newSaveDB) { sqlite3_close(newSaveDB); newSaveDB = NULL; }
}

void SqliteManager_nonReturnQueryFromSaveDB(jmethodID id, va_list args) {
    exec_non_return_query(saveDB, va_arg(args, jstring));
}
void SqliteManager_nonReturnQueryFromNewSaveDB(jmethodID id, va_list args) {
    exec_non_return_query(newSaveDB, va_arg(args, jstring));
}
void SqliteManager_setSaveDBVersion(jmethodID id, va_list args) {
    jint version = va_arg(args, jint);
    char path[256];
    snprintf(path, sizeof(path), "%ssave_version.txt", DB_BASE_PATH);
    FILE* f = fopen(path, "w");
    if (f) {
        fprintf(f, "%d", version);
        fclose(f);
    }
}

// --- Boolean Methods (Opening DBs) ---
jboolean SqliteManager_openCommonDB(jmethodID id, va_list args) { return open_db(&commonDB, "CommonDB.database"); }
jboolean SqliteManager_openResourceDB(jmethodID id, va_list args) { return open_db(&resourceDB, "ResourceDB.database"); }
jboolean SqliteManager_openSaveDB(jmethodID id, va_list args) { return open_db(&saveDB, "SaveDB.database"); }
jboolean SqliteManager_openNewSaveDB(jmethodID id, va_list args) { return open_db(&newSaveDB, "SaveDBNew.database"); }

// --- Int Methods ---
jint SqliteManager_getSaveDBVersion(jmethodID id, va_list args) {
    char path[256];
    snprintf(path, sizeof(path), "%ssave_version.txt", DB_BASE_PATH);
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    int version = fgetc(f) - '0';
    fclose(f);
    return version;
}

// --- Object Methods (Returning String[][]) ---
jobject SqliteManager_returnDataFromCommonDB(jmethodID id, va_list args) {
    return return_data_from_db(commonDB, va_arg(args, jstring));
}
jobject SqliteManager_returnDataFromResourceDB(jmethodID id, va_list args) {
    return return_data_from_db(resourceDB, va_arg(args, jstring));
}
jobject SqliteManager_returnDataFromSaveDB(jmethodID id, va_list args) {
    return return_data_from_db(saveDB, va_arg(args, jstring));
}
jobject SqliteManager_returnDataFromNewSaveDB(jmethodID id, va_list args) {
    return return_data_from_db(newSaveDB, va_arg(args, jstring));
}

jobject String_getBytes(jmethodID id, va_list args) {
    jobject this_obj = va_arg(args, jobject);
    if (!this_obj) return NULL;

    const char *str = jni->GetStringUTFChars(&jni,this_obj,NULL);
    if (!str) {
        sceClibPrintf("String_getBytes: err\n");
        return NULL;
    }

    int len = strlen(str);

    jbyteArray bArray = jni->NewByteArray(&jni, len);
    if (bArray && len > 0) {
        jni->SetByteArrayRegion(&jni, bArray, 0, len, (const jbyte *)str);
    }

    return (jobject)bArray;
}

// ============================================================================
// FalsoJNI Array Mappings
// ============================================================================

NameToMethodID nameToMethodId[] = {
    { 96, "createTextBitmap", METHOD_TYPE_VOID},
	{ 97, "getBytes", METHOD_TYPE_OBJECT },
	{ 98, "getDeviceModel", METHOD_TYPE_OBJECT },
	{ 99, "setAnimationInterval", METHOD_TYPE_VOID },
	{ 100, "getCocos2dxWritablePath", METHOD_TYPE_OBJECT },
    { 101, "setContext", METHOD_TYPE_VOID },
    { 102, "setActivity", METHOD_TYPE_VOID },
    { 103, "setTimePrivate", METHOD_TYPE_VOID },
    { 104, "closeCommonDB", METHOD_TYPE_VOID },
    { 105, "closeResourceDB", METHOD_TYPE_VOID },
    { 106, "closeSaveDB", METHOD_TYPE_VOID },
    { 107, "closeNewSaveDB", METHOD_TYPE_VOID },
    { 108, "nonReturnQueryFromSaveDB", METHOD_TYPE_VOID },
    { 109, "nonReturnQueryFromNewSaveDB", METHOD_TYPE_VOID },
    { 110, "moveReviewPage", METHOD_TYPE_VOID },
    { 111, "moveBannerPage", METHOD_TYPE_VOID },
    { 112, "playVibrator", METHOD_TYPE_VOID },
    { 113, "purchaseItem", METHOD_TYPE_VOID },
    { 114, "setSaveDBVersion", METHOD_TYPE_VOID },
    
    { 201, "openCommonDB", METHOD_TYPE_BOOLEAN },
    { 202, "openResourceDB", METHOD_TYPE_BOOLEAN },
    { 203, "openSaveDB", METHOD_TYPE_BOOLEAN },
    { 204, "openNewSaveDB", METHOD_TYPE_BOOLEAN },

    { 301, "getSaveDBVersion", METHOD_TYPE_INT },

    { 401, "returnDataFromCommonDB", METHOD_TYPE_OBJECT },
    { 402, "returnDataFromResourceDB", METHOD_TYPE_OBJECT },
    { 403, "returnDataFromSaveDB", METHOD_TYPE_OBJECT },
    { 404, "returnDataFromNewSaveDB", METHOD_TYPE_OBJECT }
};

/*
 * JNI Methods
*/
MethodsVoid methodsVoid[] = {
    { 96, createTextBitmap},
	{ 99, setAnimationInterval},
    { 101, SqliteManager_setContext },
    { 102, SqliteManager_setActivity },
    { 103, SqliteManager_setTimePrivate },
    { 104, SqliteManager_closeCommonDB },
    { 105, SqliteManager_closeResourceDB },
    { 106, SqliteManager_closeSaveDB },
    { 107, SqliteManager_closeNewSaveDB },
    { 108, SqliteManager_nonReturnQueryFromSaveDB },
    { 109, SqliteManager_nonReturnQueryFromNewSaveDB },
    { 110, SqliteManager_moveReviewPage },
    { 111, SqliteManager_moveBannerPage },
    { 112, SqliteManager_playVibrator },
    { 113, SqliteManager_purchaseItem },
    { 114, SqliteManager_setSaveDBVersion }
};

MethodsBoolean methodsBoolean[] = {
    { 201, SqliteManager_openCommonDB },
    { 202, SqliteManager_openResourceDB },
    { 203, SqliteManager_openSaveDB },
    { 204, SqliteManager_openNewSaveDB }
};

MethodsInt methodsInt[] = {
    { 301, SqliteManager_getSaveDBVersion }
};

MethodsObject methodsObject[] = {
	{ 97, String_getBytes },
	{ 98, getDeviceModel },
	{ 100, getCocos2dxWritablePath },
    { 401, SqliteManager_returnDataFromCommonDB },
    { 402, SqliteManager_returnDataFromResourceDB },
    { 403, SqliteManager_returnDataFromSaveDB },
    { 404, SqliteManager_returnDataFromNewSaveDB }
};

MethodsByte methodsByte[] = {};
MethodsChar methodsChar[] = {};
MethodsDouble methodsDouble[] = {};
MethodsFloat methodsFloat[] = {};
MethodsLong methodsLong[] = {};
MethodsShort methodsShort[] = {};

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
