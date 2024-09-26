use colorutils_rs::TransferFunction;

pub fn param_into_transfer(param: i32) -> Result<TransferFunction, String> {
    match param {
        0 => Ok(TransferFunction::Srgb),
        1 => Ok(TransferFunction::Rec709),
        2 => Ok(TransferFunction::Gamma2p2),
        3 => Ok(TransferFunction::Gamma2p8),
        _ => Err("Requested unknown colorspace".parse().unwrap()),
    }
}
