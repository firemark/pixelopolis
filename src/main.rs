extern crate image;

/*use image::{
    GenericImage, RgbImage, ImageBuffer, Rgb
};*/

mod royal_pixel;
mod royal_image;

use royal_image::{RoyalImage, PixelAttrs, Direction, fnType};
use royal_pixel::RoyalPixel;
use std::path::Path;
use std::fs::File;

const BLOCK_LENGTH:usize = 64;
const BLOCK_HEIGHT:usize = 32;

type usize2 = (usize, usize);
type usize3 = (usize, usize, usize);

fn main() {
    let img = &mut RoyalImage::new(512, 512);
    let func = |a: PixelAttrs| -> RoyalPixel {
        let cond = a.y % 4 != 0 && a.x % 8 != 0;
        let brick_color = if (cond) {
            [80u8, 25u8, 33u8]
        } else {
            [215u8; 3]
        };
        return RoyalPixel {
            pixel: brick_color, ..royal_pixel::BLACK
        }
    };
    create_cuboid(img, (256, 256), (BLOCK_LENGTH * 3, BLOCK_LENGTH, BLOCK_HEIGHT), &func);
    let path = Path::new("test.png");
    img.to_image_buffer().save(path);
}

fn create_cuboid(img: &mut RoyalImage, cor: usize2, size: usize3, cb: &fnType) {
    let (w, l, h) = size;
    create_wall(img, cor, (w, h), PixelAttrs::with_direction(Direction::Left), cb);
    create_wall(img, cor, (l, h), PixelAttrs::with_direction(Direction::Right), cb);
}

fn create_wall(img: &mut RoyalImage, cor: usize2, size: usize2, attrs: PixelAttrs, cb: &fnType) {
    let (x, y) = cor;
    let (w, h) = size;
    for n in 0..h {
        let new_cor = (x, y - n);
        let new_attrs = PixelAttrs {y: n as u32, ..attrs};
        create_bar(img, new_cor, w, new_attrs, cb);
    }
}

fn create_bar(img: &mut RoyalImage, cor: usize2, length: usize, attrs: PixelAttrs, cb: &fnType) {
    let (x, y) = cor;
    let dir = attrs.dir;
    for n in 0..length {
        let new_cor = match dir {
            Direction::Left => (x - n, y - n/2),
            Direction::Right => (x + n, y - n/2),
            Direction::Up => (x, y - n)
        };
        let new_attrs = PixelAttrs { x: n as u32, ..attrs };
        img.set_with_fn(new_cor, new_attrs, cb);   
    }
}