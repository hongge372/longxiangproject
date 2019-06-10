#include <cstdio>
#include "log.h"
#include "jniInterface.h"

static void JNICALL jniSayHello(JNIEnv *env, jobject obj){
    LOGD("wlz here success \n");
    __android_log_print(ANDROID_LOG_ERROR, "wlz", "wlz say hello" );
}

static JNINativeMethod methods[]={
        {"sayHello", "()V", (void *)jniSayHello},
};

 static int registerNatives(JNIEnv *pEnv) {
    jclass clazz = pEnv->FindClass(jniVideoEditClass);
    if(!clazz){
        return JNI_FALSE;
    }
     int methodSize = sizeof(methods) / sizeof(methods[0]);
    if(pEnv->RegisterNatives(clazz, methods, methodSize) < 0){
        LOGE("registernatives err");
        return JNI_FALSE;
    }
    return 0;
}

jint JNICALL JNI_OnLoad(JavaVM *vm, void *unused) {
    JNIEnv *env = NULL;
    jint ret = -1;
    LOGD("onLOnad");
    if(vm->GetEnv((void **)&env, JNI_VERSION_1_6) !=JNI_OK){
        return JNI_ERR;
    }
    if(registerNatives(env)!=0){
        return JNI_ERR;
    }

    ret = JNI_VERSION_1_6;
    return ret;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *unused) {
    JNIEnv *env = nullptr;
    jint ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if(ret != JNI_OK){
        return;
    }
}