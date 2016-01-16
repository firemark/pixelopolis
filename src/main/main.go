package main

import (
    "image/png"
    "os"
    "log"
    "pixelopolis"
    "math/rand"
    //"math"
    //"fmt"
)

func brick() func(a pixelopolis.PixelAttrs) pixelopolis.RoyalPixel {
    move_y := rand.Int()
    move_x := rand.Int()
    return func(a pixelopolis.PixelAttrs) pixelopolis.RoyalPixel {
        y := a.Y + move_y
        x := a.X + move_x
        cond := y % 4 != 0 && (x + (y / 4) * 3) % 8 != 0
        var brick_color [3]byte
        if (cond) {
            brick_color = [3]byte{80, 25, 33}
        } else {
            brick_color = [3]byte{215, 215, 215}
        }
        return pixelopolis.FromRGB(brick_color)  
    }
}

func main() {
    rimage := pixelopolis.NewRoyalImage(512, 512)
    drw := pixelopolis.Drawer{Img: &rimage, Fn: brick()}
    drw.CreateCylinder([2]int{256, 256}, [2]int{64, 48})
    drw.CreateCone([2]int{256, 256 - 48}, 64)
    drw.CreateCuboid([2]int{128, 128}, [3]int{32,32,64})
    drw.CreateCuboid([2]int{128, 220}, [3]int{32,32,64})
    drw.CreateCuboid([2]int{128, 300}, [3]int{32,32,64})
    file, err := os.Create("test.png")
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()
    png.Encode(file, rimage.ToImage())

}
