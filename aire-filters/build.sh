RUSTFLAGS="-C target-feature=+neon" cargo build --target aarch64-linux-android --release
#cp -r target/aarch64-linux-android/release/libaire_filters.a ../aire/src/main/cpp/lib/arm64-v8a/libaire_filters.a
cp -r target/aarch64-linux-android/release/libaire_filters.so ../aire/src/main/cpp/lib/arm64-v8a/libaire_filters.so

RUSTFLAGS="-C target-feature=+sse4.1" cargo build --target x86_64-linux-android --release
#cp -r target/x86_64-linux-android/release/libaire_filters.a ../aire/src/main/cpp/lib/x86_64/libaire_filters.a
cp -r target/x86_64-linux-android/release/libaire_filters.so ../aire/src/main/cpp/lib/x86_64/libaire_filters.so

cargo build --target armv7-linux-androideabi --release
#cp -r target/armv7-linux-androideabi/release/libaire_filters.a ../aire/src/main/cpp/lib/armeabi-v7a/libaire_filters.a
cp -r target/armv7-linux-androideabi/release/libaire_filters.so ../aire/src/main/cpp/lib/armeabi-v7a/libaire_filters.so

cargo build --target i686-linux-android --release
#cp -r target/i686-linux-android/release/libaire_filters.a ../aire/src/main/cpp/lib/x86/libaire_filters.a
cp -r target/i686-linux-android/release/libaire_filters.so ../aire/src/main/cpp/lib/x86/libaire_filters.so