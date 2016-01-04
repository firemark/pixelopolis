package main

import (
    "image/png"
    "os"
    "log"
    "pixelopolis"
    //"fmt"
)

func brick(a pixelopolis.PixelAttrs) pixelopolis.RoyalPixel {
    cond := a.Y % 4 != 0 && a.X % 8 != 0
    var brick_color [3]byte
    if (cond) {
        brick_color = [3]byte{80, 25, 33}
    } else {
        brick_color = [3]byte{215, 215, 215}
    }
    return pixelopolis.FromRGB(brick_color)
}

func main() {
    rimage := pixelopolis.NewRoyalImage(512, 512)
    drw := pixelopolis.Drawer{Img: &rimage, Fn: brick}
    drw.CreateCuboid([2]int{256, 256}, [3]int{64, 64, 48})
    file, err := os.Create("test.png")
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()
    png.Encode(file, rimage.ToImage())

}
