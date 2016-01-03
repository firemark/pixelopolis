extern crate image;

use image::Rgb;
use royal_pixel::{RoyalPixel, BLACK};
use image::RgbImage;
use utils::usize2;
use std::vec;

pub struct RoyalImage {
    pixels: Vec<RoyalPixel>,
    width: usize,
    height: usize
}

#[derive(Copy, Clone)]
pub enum Direction {
    Left, Right, Up
}

#[derive(Copy, Clone)]
pub struct PixelAttrs {
    pub x:u32,
    pub y:u32,
    pub z:u32,
    pub dir: Direction
}

pub type fnType = Fn(PixelAttrs) -> RoyalPixel;

const DEFAULT_PIXEL_ATTR: PixelAttrs = PixelAttrs {
    x: 0, y: 0, z: 0, dir: Direction::Left
};

impl PixelAttrs {
    pub fn with_direction(dir: Direction) -> PixelAttrs {
        PixelAttrs {dir: dir, ..DEFAULT_PIXEL_ATTR}
    }
}

impl RoyalImage {
    pub fn new(width: usize, height: usize) -> RoyalImage {
        RoyalImage {
            pixels: vec![BLACK; width * height],
            width: width,
            height: height,
        }
    }

    pub fn get(&self, cor: usize2) -> &RoyalPixel {
        let (x, y) = cor;
        &self.pixels[x + self.width * y]
    }

    pub fn set(&mut self, cor: usize2, pixel: RoyalPixel) {
        let (x, y) = cor;
        self.pixels[x + self.width * y] = pixel;
    }

    pub fn set_with_fn(&mut self, cor: usize2, attr: PixelAttrs, cb: Box<fnType>) {
        self.set(cor, cb(attr))
    }

    pub fn to_image_buffer(&self) -> RgbImage {
        let func = |x: u32, y: u32| {
            self.get((x as usize, y as usize)).to_pixel()
        };
        RgbImage::from_fn(self.width as u32, self.height as u32, func)
    }
}