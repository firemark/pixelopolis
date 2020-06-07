# Pixelopolis
![oblique](./docsrc/images/oblique.png)

Transform CSS-like into pixelart city in oblique (yet) projection.

Site with examples: https://pixelopolis.herokuapp.com

**WARNING** - this code is 'very beta' - please dont be angry when something doesnt work `;_;`

## Documentation

http://firemark.github.io/pixelopolis/

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
* Documentation
    * [x] HARD setup sphinx
    * [x] HARD describe elements
    * [ ] HARD write tutorial
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
    * [x] MID Grid
    * [x] MID Union
    * [x] MID Align for series
    * [x] MID Justify shapes
    * [ ] MID Margin shapes
    * [x] HARD Rotation
    * [ ] HARD City / Road / Place planner
    * [ ] HARD Python support to generate custom shapes
* Textures
    * [x] EASY Support colors instead of textures
    * [ ] EASY color names
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
    * [ ] HARD garbage collector
    * [ ] IMPOSIBRU Refactoring

## Changelog

### 0.1.1

* Upgrade documentation
* fixes in floor rendering

### 0.1.0

* Add dome shape
* Add cylinder shape
* Add cone shape
* Add justify for filler and series containers
* Add fence square container
* Add union container
* Add BSP Tree container
* Add particles system
* Add rotate attributes
* Add more textures (now textures have a few colors)
* Add more bugs, of course
* Add nested rules (like in SCSS)
* Add documenation
* Splitting code into submodules
* MORE EXAMPLES

### 0.0.1

* Everything.

## Contributors

* Firemark (forever alone)
