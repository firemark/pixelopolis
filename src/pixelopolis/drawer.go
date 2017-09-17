package pixelopolis

import "math"

const ALPHA float64 = 45.0
const RATIO float64 = 2.0 / 3.0

type FnAttrsType func(PixelAttrs) RoyalPixel

type Drawer struct {
    Img *RoyalImage
    Fn FnAttrsType
}

func (drw *Drawer) setPixel(x float64, y float64, attrs PixelAttrs) {
    fx := int(math.Floor(x))
    fy := int(math.Floor(y))
    cx := fx + 1
    cy := fy + 1
    dx := x - float64(fx)
    dy := y - float64(fy)
    dix := 1.0 - dx
    diy := 1.0 - dy

    rpixel := drw.Fn(attrs)

    ccp := *drw.Img.Get(cx, cy)
    ffp := *drw.Img.Get(fx, fy)
    cfp := *drw.Img.Get(cx, fy)
    fcp := *drw.Img.Get(fx, cy)

    drw.Img.Set(cx, cy, rpixel.Mix(ccp, dx * dy))
    drw.Img.Set(fx, fy, rpixel.Mix(ffp, dix * diy))
    drw.Img.Set(cx, fy, rpixel.Mix(cfp, dx * diy))
    drw.Img.Set(fx, cy, rpixel.Mix(fcp, dix * dy))
}

func (drw Drawer) CreateCuboid(cor [2]int, size [3]int) {
    w := size[0]
    l := size[1]
    h := size[2]
    first_cor := [2]int{cor[0] + w, cor[1]}
    drw.CreateWall(first_cor, [2]int{l, h}, PixelAttrs{Dir: East})
    drw.CreateWall(cor, [2]int{w, h}, PixelAttrs{Dir: South})
}

func (drw Drawer) CreateWall(cor [2]int, size [2]int, attrs PixelAttrs) {
    for n := 0; n < size[1]; n++ {
        attrs.Y = n
        cor[1] -= 1
        drw.CreateBar(cor, size[0], attrs)
    }
}

func cabinetProjection(z float64) (float64, float64) {
    alpha := ALPHA * math.Pi / 180.0
    cos_alpha := math.Cos(alpha)
    sin_alpha := math.Sin(alpha)

    return z * cos_alpha * RATIO, z * sin_alpha * RATIO
}

func (drw Drawer) CreateBar(cor [2]int, length int, attrs PixelAttrs) {
    x := float64(cor[0])
    y := float64(cor[1])
    float_length := float64(length)

    for n := 0.0; n < float_length; n += 0.25 {
        var nx, ny float64
        switch attrs.Dir {
        case South:
            nx, ny = x + n, y
        case East:
            tx, ty := cabinetProjection(n)
            nx, ny = x + tx, y - ty
        case Up:
            nx, ny = x, y - n
        }
        attrs.X = int(n)
        drw.setPixel(nx, ny, attrs)
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
    for n := -half_length + 0.25; n < half_length; n += 0.25 {
        z := math.Sqrt(half_length * half_length - n * n)
        //tx, ty := cabinetProjection(z)
        tx, ty := 0.0, z / 2.0
        attrs.X = int(n + half_length)
        drw.setPixel(float64(x) + tx + n, float64(y) - ty, attrs)
        attrs.X = int(n)
        drw.setPixel(float64(x) + tx + n, float64(y) + ty, attrs)
    }
}
