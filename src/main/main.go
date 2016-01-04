package main

import (
    "image/png"
    "os"
    "log"
    "pixelopolis"
    //"fmt"
)

func main() {
    rimage := pixelopolis.NewRoyalImage(512, 512)
    rimage.Set(255, 255, pixelopolis.FromRGB([3]byte{255, 255, 255}))
    file, err := os.Create("test.png")
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()
    png.Encode(file, rimage.ToImage())

}
