set -e

RUSTFLAGS="-C link-arg=-Wl,-z,max-page-size=16384 -C target-feature=+neon -C opt-level=3 -C strip=symbols" cargo +nightly build --target aarch64-linux-android --release
#cp -r target/aarch64-linux-android/release/libaire_filters.a ../aire/src/main/cpp/lib/arm64-v8a/libaire_filters.a
cp -r target/aarch64-linux-android/release/libaire_filters.so ../aire/src/main/cpp/lib/arm64-v8a/libaire_filters.so
#
RUSTFLAGS="-C link-arg=-Wl,-z,max-page-size=16384 -C target-feature=+sse4.1 -C opt-level=3 -C strip=symbols" cargo +nightly build --target x86_64-linux-android --release
##cp -r target/x86_64-linux-android/release/libaire_filters.a ../aire/src/main/cpp/lib/x86_64/libaire_filters.a
cp -r target/x86_64-linux-android/release/libaire_filters.so ../aire/src/main/cpp/lib/x86_64/libaire_filters.so
#
RUSTFLAGS="-C link-arg=-Wl,-z,max-page-size=16384 -C strip=symbols" cargo +nightly build --target armv7-linux-androideabi --release
##cp -r target/armv7-linux-androideabi/release/libaire_filters.a ../aire/src/main/cpp/lib/armeabi-v7a/libaire_filters.a
cp -r target/armv7-linux-androideabi/release/libaire_filters.so ../aire/src/main/cpp/lib/armeabi-v7a/libaire_filters.so

RUSTFLAGS="-C link-arg=-Wl,-z,max-page-size=16384 -C strip=symbols" cargo +nightly build --target i686-linux-android --release
##cp -r target/i686-linux-android/release/libaire_filters.a ../aire/src/main/cpp/lib/x86/libaire_filters.a
cp -r target/i686-linux-android/release/libaire_filters.so ../aire/src/main/cpp/lib/x86/libaire_filters.so