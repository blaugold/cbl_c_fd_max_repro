#include "cbl/CouchbaseLite.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include "jni.h"
#include <android/log.h>

static CBLDatabase *db = nullptr;
static CBLReplicator *repl = nullptr;

// Leaks [fdCount] file descriptors.
static void openFileDescriptors(int fdCount)
{
    for (int i = 0; i < fdCount; i++)
    {
        new std::ofstream("/dev/null");
    }

    __android_log_print(ANDROID_LOG_INFO, "native-lib", "Opened %d file descriptors\n", fdCount);
}

static void openDatabase()
{
    db = CBLDatabase_Open(FLStr("db"), nullptr, nullptr);
    assert(db);
}

static void createReplicator()
{
    auto endpoint = CBLEndpoint_CreateWithURL(FLStr("ws://localhost:5000/db"), nullptr);
    assert(endpoint);

    CBLReplicatorConfiguration config{
        .database = db,
        .endpoint = endpoint,
        .maxAttempts = 1,
    };

    repl = CBLReplicator_Create(&config, nullptr);
    assert(repl);
    CBLEndpoint_Free(endpoint);
}

static void runReplicator()
{
    using namespace std::chrono_literals;

    CBLReplicator_Start(repl, false);

    while (CBLReplicator_Status(repl).activity != kCBLReplicatorStopped)
    {
        std::this_thread::sleep_for(100ms);
    }
}

static void cleanUp()
{
    CBLReplicator_Release(repl);

    auto closedDb = CBLDatabase_Close(db, nullptr);
    assert(closedDb);
    CBLDatabase_Release(db);
}

static void CBLLogToLogCat(CBLLogDomain domain,
                           CBLLogLevel level,
                           FLString message)
{
    __android_log_print(ANDROID_LOG_INFO, "CBL", "%.*s\n", message.size, message.buf);
}

extern "C" void Java_com_example_cbl_1c_1fd_1max_1demo_NativeLib_initCbl(JNIEnv *env, jclass thiz,
                                                                         jstring files_dir)
{
    jboolean isCopy;
    auto files_dir_c = env->GetStringUTFChars(files_dir, &isCopy);

    auto initialized = CBL_Init({.filesDir = files_dir_c, .tempDir = files_dir_c}, nullptr);
    assert(initialized);

    env->ReleaseStringUTFChars(files_dir, files_dir_c);

    CBLLog_SetConsoleLevel(kCBLLogNone);
    CBLLog_SetCallback(&CBLLogToLogCat);
    CBLLog_SetCallbackLevel(kCBLLogVerbose);
}

extern "C" void
Java_com_example_cbl_1c_1fd_1max_1demo_NativeLib_runDemo(JNIEnv *env, jclass thiz, jint fd_count)
{
    openFileDescriptors(fd_count);
    openDatabase();
    createReplicator();
    runReplicator();
    cleanUp();
}
