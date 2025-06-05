pub fn param_into_libblur_transfer(param: i32) -> Result<libblur::TransferFunction, String> {
    match param {
        0 => Ok(libblur::TransferFunction::Srgb),
        1 => Ok(libblur::TransferFunction::Rec709),
        2 => Ok(libblur::TransferFunction::Gamma2p2),
        3 => Ok(libblur::TransferFunction::Gamma2p8),
        _ => Err("Requested unknown colorspace".parse().unwrap()),
    }
}
