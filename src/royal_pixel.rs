extern crate image;
use image::Rgb;

#[warn(dead_code)]
#[derive(Clone, Debug)]
pub struct RoyalPixel {
    pub pixel: [u8; 3],
    pub normal: [f32; 3],
    pub zbuffer: u8
}

pub const BLACK: RoyalPixel = RoyalPixel {
    pixel: [0; 3],
    normal: [0.0; 3],
    zbuffer: 0
};

pub const WHITE: RoyalPixel = RoyalPixel {
    pixel: [255; 3], ..BLACK
};

impl RoyalPixel {
    pub fn to_pixel(&self) -> Rgb<u8> {
        Rgb(self.pixel)
    }
}