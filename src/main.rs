extern crate image;

/*use image::{
    GenericImage, RgbImage, ImageBuffer, Rgb
};*/

mod royal_pixel;
mod royal_image;

use royal_image::RoyalImage;
use royal_pixel::RoyalPixel;
use std::path::Path;
use std::fs::File;

#[derive(Copy, Clone)]
enum Direction {
    Left, Right, Up
}

const BLOCK_LENGTH:usize = 64;
const BLOCK_HEIGHT:usize = 32;

fn main() {
    let img = &mut RoyalImage::new(512, 512);
    create_cuboid(img, (256, 256), (1, 1));
    let img_buff = img.to_image_buffer();
    let path = Path::new("test.png");
    //let ref mut fout = File::create().unwrap();
    let _ = img_buff.save(path);
}

fn create_cuboid(img: &mut RoyalImage, cor: (usize, usize), size: (usize, usize)) {
    create_wall(img, cor, BLOCK_HEIGHT, Direction::Left);
    create_wall(img, cor, BLOCK_HEIGHT, Direction::Right);
}

fn create_wall(img: &mut RoyalImage, cor: (usize, usize), length: usize, dir: Direction) {
    let (x, y) = cor;
    for n in 0..length {
        let new_cor = (x, y - n);
        create_bar(img, new_cor, BLOCK_LENGTH, dir);
    }
}

fn create_bar(img: &mut RoyalImage, cor: (usize, usize), length: usize, dir: Direction) {
    let (x, y) = cor;
    for n in 0..length {
        let new_cor = match dir {
            Direction::Left => (x - n, y - n/2),
            Direction::Right => (x + n, y - n/2),
            Direction::Up => (x, y - n)
        };
        let nn = n as u8;
        img.set(new_cor, RoyalPixel { pixel: [nn + 32, nn + 32, nn*2], ..royal_pixel::BLACK });   
    }
}