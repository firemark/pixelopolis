package pixelopolis

import "math"

type fnAttrsType func(PixelAttrs) RoyalPixel

type Drawer struct {
    Img *RoyalImage 
    Fn fnAttrsType
}

func (drw Drawer) CreateCuboid(cor [2]int, size [3]int) {
    w := size[0]
    l := size[1]
    h := size[2]
    drw.CreateWall(cor, [2]int{w, h}, PixelAttrs{Dir: Left})
    drw.CreateWall(cor, [2]int{l, h}, PixelAttrs{Dir: Right})
}

func (drw Drawer) CreateWall(cor [2]int, size [2]int, attrs PixelAttrs) {
    for n := 0; n < size[1]; n++ {
        attrs.Y = n
        cor[1] -= 1
        drw.CreateBar(cor, size[0], attrs)
    }
}

func (drw Drawer) CreateBar(cor [2]int, length int, attrs PixelAttrs) {
    x := cor[0]
    y := cor[1]
    for n := 0; n < length; n++ {
        var nx, ny int
        switch attrs.Dir {
        case Left:
            nx, ny = x - n, y - n/2
        case Right:
            nx, ny = x + n, y - n/2 
        case Up:
            nx, ny = x, y - n
        }
        attrs.X = n
        drw.Img.Set(nx, ny, drw.Fn(attrs))
    }
}


func (drw Drawer) CreateCylinder(cor [2]int, size [2]int) {
    attrs := PixelAttrs{}
    for n := 0; n < size[1]; n++ {
        attrs.Y = n
        cor[1] -= 1
        drw.CreateCircleBar(cor, size[0], attrs)
    }
}

func (drw Drawer) CreateCone(cor [2]int, size int) {
    attrs := PixelAttrs{}
    for n := 0; n < size; n++ {
        attrs.Y = n
        cor[1] -= 1
        drw.CreateCircleBar(cor, size - n, attrs)
    }
}

func (drw Drawer) CreateCircleBar(cor [2]int, length int, attrs PixelAttrs) {
    x := cor[0]
    y := cor[1]
    half_length := float64(length) / 2.0
    f_half_length := int(math.Floor(half_length))
    c_half_length := int(math.Ceil(half_length))
    for n := -c_half_length; n < f_half_length; n++ {
        //nx := x - n
        float_n := float64(n)
        fy := float64(y) + math.Sqrt(half_length*half_length - float_n * float_n) / 2.0
        fny := int(math.Floor(fy))
        cny := int(math.Ceil(fy))
        attrs.X = n
        drw.Img.Set(x + n, fny, drw.Fn(attrs))
        drw.Img.Set(x + n, cny, drw.Fn(attrs))
    }
}