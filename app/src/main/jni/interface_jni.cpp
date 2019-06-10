#include <cstdio>
#include "log.h"
#include "jniInterface.h"
#include <memory>
#include "ahvfx_applyer.hpp"

std::shared_ptr<ahvfx::applyer> app;
jint JNICALL native_use_sticker(JNIEnv *env, jobject obj, jstring folder, jstring name) {
    if (app == 0) {
        app = ahvfx::applyer::create();
    }
    char* c_folder = (char*) env->GetStringUTFChars(folder, 0);
    char* c_name = (char*) env->GetStringUTFChars(name, 0);
    ahvfx::fxmodel(c_folder, c_name);
    //app->use(fxmodel);
    env->ReleaseStringUTFChars(folder, c_folder);
    env->ReleaseStringUTFChars(name, c_name);
    return 1;
}




static JNINativeMethod methods[]={
        {"_native_use_sticker", "(Ljava/lang/String;Ljava/lang/String;)I",(void *) native_use_sticker},

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