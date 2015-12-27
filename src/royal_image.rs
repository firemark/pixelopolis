extern crate image;

use image::Rgb;
use royal_pixel::{RoyalPixel, BLACK};
use image::RgbImage;
use std::vec;

pub struct RoyalImage {
    pixels: Vec<RoyalPixel>,
    width: usize,
    height: usize
}

impl RoyalImage {
    pub fn new(width: usize, height: usize) -> RoyalImage {
        RoyalImage {
            pixels: vec![BLACK; width * height],
            width: width,
            height: height,
        }
    }

    pub fn get(&self, cor: (usize, usize)) -> &RoyalPixel {
        let (x, y) = cor;
        &self.pixels[x + self.width * y]
    }

    pub fn set(&mut self, cor: (usize, usize), pixel: RoyalPixel) {
        let (x, y) = cor;
        self.pixels[x + self.width * y] = pixel;
    }

    pub fn to_image_buffer(&self) -> RgbImage {
        let func = |x:u32, y:u32| { self.get((x as usize, y as usize)).to_pixel() };
        RgbImage::from_fn(self.width as u32, self.height as u32, func)
    }
}