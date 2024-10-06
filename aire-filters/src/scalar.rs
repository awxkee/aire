#[cfg(target_os = "android")]
#[allow(non_snake_case)]
pub mod android {
    extern crate jni;

    use jni::objects::JObject;
    use jni::sys::jobject;
    use jni::JNIEnv;
    use libblur::Scalar;

    pub fn get_scalar_from_java(env: &mut JNIEnv, scalar_class: jobject) -> Scalar {
        let new_obj = unsafe { JObject::from_raw(scalar_class) };
        let r_field_val = env.get_field(&new_obj, "r", "D").unwrap().d().unwrap();
        let g_field_val = env.get_field(&new_obj, "g", "D").unwrap().d().unwrap();
        let b_field_val = env.get_field(&new_obj, "b", "D").unwrap().d().unwrap();
        let a_field_val = env.get_field(&new_obj, "a", "D").unwrap().d().unwrap();
        Scalar::new(r_field_val, g_field_val, b_field_val, a_field_val)
    }
}
