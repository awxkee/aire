//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

#include <jni.h>
#include <stdexcept>
#include <string>
#include <android/log.h>

class AireError : public std::runtime_error {
    std::string what_message;

public:
    AireError(const std::string &str) : runtime_error(str), what_message(str) {

    }

    const char *what() const noexcept override {
        return what_message.c_str();
    }
};

static jint throwException(JNIEnv *env, std::string &msg) {
    jclass exClass;
    exClass = env->FindClass("java/lang/Exception");
    return env->ThrowNew(exClass, msg.c_str());
}

static uint32_t swapByteOrder32(uint32_t big) {
    uint32_t little = ((big & 0xFF) << 24)
                      | ((big & 0xFF00) << 8)
                      | ((big >> 8) & 0xFF00)
                      | (big >> 24);
    return little;
}

#define LOG_TAG "Aire"
#define LOG(severity, ...) ((void)__android_log_print(ANDROID_LOG_##severity, LOG_TAG, __VA_ARGS__))
#define LOGE(...) LOG(ERROR, __VA_ARGS__)
#define LOGV(...) LOG(VERBOSE, __VA_ARGS__)
