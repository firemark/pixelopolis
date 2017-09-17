package pixelopolis

import "image/color"

type RoyalPixel struct {
    Color [3]byte
    Normal [3]float32
    Zbuffer byte
}

var black = FromRGB([3]byte{0, 0, 0})
var white = FromRGB([3]byte{255, 255, 255})

func NewRoyalPixel() RoyalPixel {
    return RoyalPixel{
        Color: [3]byte{0, 0, 0},
        Normal: [3]float32{0.0, 0.0, 0.0},
        Zbuffer: byte(0),
    }
}

func FromRGB(rgb [3]byte) RoyalPixel {
    rpixel := NewRoyalPixel()
    rpixel.Color = rgb
    return rpixel
}

func (rpixel RoyalPixel) ToRGBA() color.RGBA {
    return color.RGBA {
        R: rpixel.Color[0],
        G: rpixel.Color[1],
        B: rpixel.Color[2],
        A: byte(255),
    }
}

func (rpixel RoyalPixel) CopyWithoutColor() RoyalPixel {
    new_pixel := NewRoyalPixel()
    new_pixel.Normal = rpixel.Normal
    new_pixel.Zbuffer = rpixel.Zbuffer
    return new_pixel
}

func (rpixel RoyalPixel) Mix(another_rpixel RoyalPixel, c float64) RoyalPixel {
    new_pixel := rpixel.CopyWithoutColor()
    for i := 0; i < 3; i++ {
        color := float64(another_rpixel.Color[i]) * (1.0 - c)
        color += float64(rpixel.Color[i]) * c
        new_pixel.Color[i] = byte(color)
    }

    return new_pixel
}
