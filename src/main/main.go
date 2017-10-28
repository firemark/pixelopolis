package main

import (
    "image/png"
    "image/color"
    "os"
    "log"
    "pixelopolis"
    "math/rand"
    //"math"
    //"fmt"
)

func brick() pixelopolis.FnAttrsType {
    move_x := rand.Int()
    return func(a pixelopolis.PixelAttrs) pixelopolis.RoyalPixel {
        y := a.Y
        x := a.X + move_x
        cond := y % 4 != 0 && (x + (y / 4) * 4) % 8 != 0
        var brick_color [3]byte
        if (cond) {
            brick_color = [3]byte{80, 25, 33}
        } else {
            brick_color = [3]byte{215, 215, 215}
        }
        return pixelopolis.FromRGB(brick_color)
    }
}

func blue(a pixelopolis.PixelAttrs) pixelopolis.RoyalPixel {
    return pixelopolis.FromRGB([3]byte{30, 144, 255})
}

func image_texture(path string, scale int) (pixelopolis.FnAttrsType, error) {
    reader, err := os.Open(path)
    if err != nil {
        return nil, err
    }
    defer reader.Close()
    image, err := png.Decode(reader)
    if err != nil {
        return nil, err
    }
    bounds := image.Bounds()
    width := bounds.Max.X
    height := bounds.Max.Y
    lambda := func(a pixelopolis.PixelAttrs) pixelopolis.RoyalPixel {
        x := (a.X * scale) % width
        y := (-a.Y * scale) % height
        if x < 0 { x += width }
        if y < 0 { y += height }
        rawColor := image.At(x, y)
        var pixelColor [3]byte
        switch pixel := rawColor.(type) {
        case color.RGBA:
            pixelColor = [3]byte{pixel.R, pixel.G, pixel.B}
        case color.NRGBA:
            pixelColor = [3]byte{pixel.R, pixel.G, pixel.B}
        default:
            panic("Image has no RGBA format")
        }
        return pixelopolis.FromRGB(pixelColor)
    }
    return lambda, nil
}

func main() {
    rimage := pixelopolis.NewRoyalImage(512, 512)
    texture, err := image_texture("tex.png", 1)
    if err != nil {
        log.Fatal(err)
    }
    textureBrick, err := image_texture("brick.png", 1)
    if err != nil {
        log.Fatal(err)
    }
    drw := pixelopolis.Drawer{Img: &rimage, Fn: texture}
    drw.CreateCuboid([2]int{128, 220}, [3]int{48, 32, 48})

    drwBrick := pixelopolis.Drawer{Img: &rimage, Fn: textureBrick}
    drwBrick.CreateCylinder([2]int{256, 256}, [2]int{64, 48})
    drwBrick.CreateCone([2]int{256, 256 - 48}, 64)

    another_drw := pixelopolis.Drawer{Img: &rimage, Fn: blue}
    another_drw.CreateCuboid([2]int{128, 300}, [3]int{32,32,64})

    file, err := os.Create("test.png")
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()
    png.Encode(file, rimage.ToImage())

}
