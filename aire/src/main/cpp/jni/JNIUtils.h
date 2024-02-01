//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

#include <jni.h>
#include <stdexcept>
#include <string>

class AireError : public std::runtime_error {
    std::string what_message;

public:
    AireError(const std::string &str) : runtime_error(str), what_message(str) {

    }

    const char * what() const noexcept override {
        return what_message.c_str();
    }
};

static jint throwException(JNIEnv *env, std::string &msg) {
    jclass exClass;
    exClass = env->FindClass("java/lang/Exception");
    return env->ThrowNew(exClass, msg.c_str());
}