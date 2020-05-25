# Pixelopolis
![oblique](./images/oblique.png)

Transform CSS-like into pixelart city in oblique (yet) projection.

Site with examples: https://pixelopolis.herokuapp.com

**WARNING** - this code is 'very beta' - please dont be angry when something doesnt work `;_;`

## Howto install
```bash
make all
./pixelopolis
```

### Docker version (with web panel)

```bash
docker build . -t pixelopolis
docker run -it --rm -p 8000:80 pixelopolis
```

## Howto run (without docker)

```bash
./pixelopolis [CSS_IN_FILE] [PNG_OUT_FILE]
```

### Body

The main element, required if you need run a program
```css
body {
    body: DRAW-OBJ-SELECTOR;
    width: NUMBER; /* width of output image */
    height: NUMBER; /* height of output image */
}
```

### DrawObj

The 3D objects like cube, pyramid, row/column of objects.
Each drawObj has width/height/depth properties

```css
draw-obj {
    width: NUMBER | PERCENT; /* default: 0 */
    height: NUMBER | PERCENT; /* default: 0 */
    depth: NUMBER | PERCENT; /* default: 0 */
}
```

This are 2 ways to make CSS rule for drawObj:

```css
/* first way */ draw-obj { /* ... */ }
/* second way */ customize-element-name { display: draw-obj; /* ... */ }
/* display has bigger priority than element name */
```

#### Basic

##### Void

Just empty cube. Nice to shift another element

```css
void {
    child: SELECTOR-TO-DRAW-OBJ;
    width: NUMBER | PERCENT;
    height: NUMBER | PERCENT;
    depth: NUMBER | PERCENT;
}
```

##### Cube

```css
cube {
    wall: SELECTOR-TO-WALL-OBJ;
    roof: SELECTOR-TO-WALL-OBJ;
    width: NUMBER | PERCENT;
    height: NUMBER | PERCENT;
    depth: NUMBER | PERCENT;
}
```

##### Triangle

```css
triangle {
    wall: SELECTOR-TO-WALL-OBJ;
    roof: SELECTOR-TO-WALL-OBJ;
    width: NUMBER | PERCENT;
    height: NUMBER | PERCENT;
    depth: NUMBER | PERCENT;
}
```

##### Pyramid

```css
pyramid {
    wall: SELECTOR-TO-WALL-OBJ;
    width: NUMBER | PERCENT;
    height: NUMBER | PERCENT;
    depth: NUMBER | PERCENT;
}
```

#### Containers

Basic Shapes are not enough, we need to show many objects.

##### Series

Is a CSS rule with list of another draw-obj selectors in given axis.
We have a tree types of series obj:

* `v-series` - vertical (on x-axis)
* `d-series` - depth (on y-axis)
* `h-series` - height (on z-axis)

```css
v-series { /* or d-series or h-series */
    body: SELECTOR1, SELECTOR2, ...;
    padding: NUMBER; /* optional */
    width: NUMBER | PERCENT; /* optional */
    height: NUMBER | PERCENT; /* optional */
    depth: NUMBER | PERCENT; /* optional */
    align: start | end | center | stretch; /* default is start */
}
```

##### Filler

Instead of series, Filler fills whole specific space (if can)
We have a tree types of fillers obj:

* `v-filler` - vertical (on x-axis)
* `d-filler` - depth (on y-axis)
* `h-filler` - height (on z-axis)

```css
v-filler {
    body: SELECTOR;
    padding: NUMBER; /* optional */
    width: NUMBER | PERCENT; /* optional */
    height: NUMBER | PERCENT; /* optional */
    depth: NUMBER | PERCENT; /* optional */
    align: start | end | center | stretch; /* default is start */
}
```

### Wall

Rule to draw texture and specified floors on the wall. Used in objects.

```css
wall {
    texture: SELECTOR-TO-TEX-OBJ;
    top: SELECTOR-TO-FLOOR-OBJ; /* optional */
    middle: SELECTOR-TO-FLOOR-OBJ; /* optional */
    bottom: SELECTOR_TO_FLOOR_OBJ; /* optional */
    padding: NUMBER; /* optional - padding between floors */
}
```

### Floor

Rule to draw textures or/and set height on the floor.

```css
floor {
    left: SELECTOR-TO-TEX-OBJ;
    middle: SELECTOR-TO-TEX-OBJ;
    right: SELECTOR-TO-TEX-OBJ;
    padding: NUMBER; /* optional - padding between textures, default is 0 */
    left-padding: NUMBER; /* optional - default is from padding attribute */
    right-padding: NUMBER; /* optional - default is from padding attribute */
    middle-padding: NUMBER;  /* optional - default is from padding attribute */
}
```

### Texture

Rule to load image from file

```css
tex {
    texture: STRING;
}
```

## TODOS

* Rendering System
    * [ ] EASY Background color/transparency
    * [ ] EASY Scale output image
    * [ ] EASY Random seed
    * [ ] MID More projections support (like isometric)
    * [ ] MID More layers export (Normal/Zindex/Color)
    * [ ] MID Limited Pallette colors
    * [ ] MID Outline
    * [ ] HARD Better Shading
    * [ ] HARD 2.5D Background Mountains and floor
    * [ ] HARD 2.5D Clouds
    * [ ] NO-MERCY Opengl + shaders support
* CSS Parser
    * [x] EASY CSS Comments
    * [ ] EASY Star operator
    * [ ] MID Nth child pseudoclass
    * [ ] MID Imports
    * [ ] MID Class joins
    * [ ] HARD Variables
    * [ ] HARD if/switch
    * [ ] HARD SASS-like syntax
    * [ ] HARD Custom parameters in value
    * [x] HARD Parent CSS selector
* Shapes
    * [x] EASY Cube
    * [x] EASY Triangle
    * [x] EASY Pyramid
    * [ ] EASY Half-Triangle
    * [x] MID Cone
    * [x] MID Cylinder
    * [x] MID Dome
    * [ ] MID Stairs
    * [ ] MID Trapezoid
    * [ ] MID Trapezoid+Stairs
    * [ ] MID Arc
    * [x] MID Series
    * [x] MID Fillers
    * [x] MID Square Fences
    * [ ] MID Grid
    * [ ] MID Union
    * [x] MID Align for series
    * [x] MID Justify shapes
    * [ ] MID Margin shapes
    * [x] HARD Rotation
    * [ ] HARD City / Road / Place planner
    * [ ] HARD Python support to generate custom shapes
* Textures
    * [x] EASY Support colors instead of textures
    * [ ] MID Gradient in textures
    * [ ] MID Point noise
    * [ ] MID Perlin noise
    * [ ] MID Grid pattern
    * [ ] MID Brick pattern
    * [ ] MID Tile pattern
    * [ ] MID Chess pattern
    * [ ] MID Wave pattern
    * [ ] MID Spiral pattern
    * [ ] MID Disc pattern
    * [ ] MID Cross pattern
    * [ ] MID Bars pattern
    * [x] MID Flat sprites (like in DOOM game) like grass, items, people etc.
    * [x] MID Point clouds
    * [ ] MID Point clouds parameters
    * [ ] HARD Rock pattern
    * [ ] HARD Edges
    * [ ] HARD Simple generators
    * [ ] HARD Mixing textures
    * [ ] HARD Better scaling/rotating textures
    * [ ] HARD Normal shader for textures
    * [ ] HARD Bump map to normal shader
* Core
    * [ ] HARD Make sphinx documentation on github pages
    * [ ] HARD garbage collector
    * [ ] IMPOSIBRU Refactoring

## Changelog

### 0.0.1

* Everything.

## Contributors

* Firemark (forever alone)
