package pixelopolis

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
        new_cor := [2]int{cor[0], cor[1] - n}
        drw.CreateBar(new_cor, size[0], attrs)
    }
}

func (drw Drawer) CreateBar(cor [2]int, length int, attrs PixelAttrs) {
    x := cor[0]
    y := cor[1]
    for n := 0; n < length; n++ {
        var nx, ny int
        switch attrs.Dir {
        case Left: nx, ny = x - n, y - n/2
        case Right: nx, ny = x + n, y - n/2 
        case Up: nx, ny = x, y - n
        }
        attrs.X = n
        drw.Img.Set(nx, ny, drw.Fn(attrs))
    }
}