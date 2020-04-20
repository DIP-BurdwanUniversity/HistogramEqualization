## Classic C code for performing histogram equalization

#### Description

The C code can equalize a single colour channel at a time among 3 *[R,G,B]* Channels in a 24 bit image.

This is described at line 120 as:
```
for(i=0; i<width*height; i++) {
        image[i].r = DF[*(pixel_arr+i)];    // red channel equalization
}
```

#### Inputs and outputs

+ Sample inputs are in `input` directory
+ Sample outputs are in `output` directory

#### Screenshots

![Output terminal](misc\term.png)
