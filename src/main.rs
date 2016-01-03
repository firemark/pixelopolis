extern crate image;

mod royal_pixel;
mod royal_image;
mod drawer;
mod utils;

use std::path::Path;
use std::fs::File;

use royal_image::{RoyalImage, PixelAttrs};
use royal_pixel::RoyalPixel;
use drawer::Drawer;

const BL:usize = 64;
const BH:usize = 32;

fn main() {
    let mut img = RoyalImage::new(512, 512);
    let func = |a: PixelAttrs| -> RoyalPixel {
        let cond = a.y % 4 != 0 && a.x % 8 != 0;
        let brick_color = if cond {[80u8, 25u8, 33u8]} else {[215u8; 3]};

        return RoyalPixel {pixel: brick_color, ..royal_pixel::BLACK}
    };
    {
        let drw = Drawer::new(Box::new(img), Box::new(func));
        drw.create_cuboid((256, 256), (BL * 3, BL, BH));
        drw.create_cuboid((128, 128), (BH, BL, BH));
    }

    let path = Path::new("test.png");
    img.to_image_buffer().save(path);
}
