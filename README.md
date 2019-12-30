# Pixelopolis
![oblique](./images/oblique.png)

Transform CSS-like into pixelart city in oblique (yet) projection.

Directory with images: ?

Site with examples: ?

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

## CSS 

### Structure

Because we don't have XML/HTML, so we need make structure from CSS.

Actual structure:

```
body
└── draw-obj (cube, pyramid, series, filler, etc...)
    └── wall
        └── floor
            └── texture
```

### Schema

Pixelopolis has css-like syntax (not fully supported CSS2, *yet*)

#### Rule
```css
RULE {
    property: <OBJ>;
    property: <OBJ1>, <OBJ2>; /* array of N elements */
    property: <OBJ>;
    /* ... */
}
```

#### Selectors

```css
element { /*...properties...*/ }
element.class { /*...properties...*/ }
element.class1.class2 { /*...properties...*/ }
parent element { /*...properties...*/ } 
parent > element { /*...properties...*/ }
```

#### Properties

##### Number

```css
rule { 
    number: 100;
    math: 100 * (2 + 2); /* mathematical operators are supported */
}
```

##### Percents

```css
rule { 
    full-percent: 100%;
    half-percent: 50%;
}
```

#### Strings

```css
rule {
    string: "foobar";
}

##### Selectors

```css
rule { 
    child: child-rule.child-class;
    another_child: child-rule;
    children: child-rule, child-rule.class, child-rule;
}
```

##### Functions
```css
rule { 
    random_number: random(0, 50); 
    random_obj: choice(obj1, obj2, obj3);
} 
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
    width: NUMBER | PERCENT; /* default: 50 */
    height: NUMBER | PERCENT; /* default: 50 */
    depth: NUMBER | PERCENT; /* default: 50 */
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
    width: NUMBER / PERCENT; /* optional */
    height: NUMBER / PERCENT; /* optional */
    depth: NUMBER / PERCENT; /* optional */
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
}
```

### Wall

### Floor

### Texture

## TODOS

* [ ] EASY More DrawObj structures like city wall, stairs, cone, cylinder etc.
* [ ] EASY CSS Comments
* [ ] HARD City / Road / Place planner
* [ ] HARD Customizing shape of buildings
* [x] HARD Parent CSS selector
* [ ] MID More projections like perspective
* [ ] MID Flat sprites (like in DOOM game) like grass, trees etc
* [ ] MID More attributes for DrawObj like customize padding, margin 
* [ ] MID Mixing textures
* [ ] HARD Normal shader for textures
* [ ] HARD rotation
* [ ] HARD Sass-like operator
* [ ] NO-MERCY Opengl + shaders support
* [ ] IMPOSIBRU Refactoring

## Contributors

* Firemark (forever alone)
