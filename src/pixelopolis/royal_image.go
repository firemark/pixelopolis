package pixelopolis

import "image"

type RoyalImage struct {
    Pixels []RoyalPixel
    Width int
    Height int
}

type Direction int
const (
    South Direction = 0
    East Direction = 1
    Up Direction = 2
    //Never One Direction plz
)

type PixelAttrs struct {
    X, Y, Z int
    Normal []float32
    Dir Direction
}

func NewRoyalImage(width int, height int) RoyalImage {
    return RoyalImage {
        Pixels: make([]RoyalPixel, width * height),
        Width: width,
        Height: height,
    }
}

func (rimage RoyalImage) Get(x int, y int) *RoyalPixel {
    return &rimage.Pixels[x + y*rimage.Width]
}

func (rimage *RoyalImage) Set(x int, y int, rpixel RoyalPixel) {
    rimage.Pixels[x + y*rimage.Width] = rpixel
}


func (rimage RoyalImage) ToImage() *image.RGBA {
    img := image.NewRGBA(image.Rect(0, 0, rimage.Width, rimage.Height))
    for y := 0; y < rimage.Height; y++ {
        for x := 0; x < rimage.Width; x++ {
            rpixel := rimage.Get(x, y)
            img.Set(x, y, rpixel.ToRGBA())
        }
    }
    return img
}
