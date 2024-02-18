#include <jni.h>
#include <string>
#include <omp.h>
#include <thread>
#include <algorithm>

extern "C"
JNIEXPORT void JNICALL
Java_com_awxkee_aire_Aire_initializeLibrary(JNIEnv *env, jobject thiz) {
    omp_set_num_threads(std::clamp(static_cast<int>(std::thread::hardware_concurrency()), int(0), int(18)));
    omp_set_dynamic(1);
    omp_set_nested(1);
}