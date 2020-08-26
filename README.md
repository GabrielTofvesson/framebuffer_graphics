# framebuffer graphics
This is my stupid attempt at drawing directly to the Linux framebuffer.

The framebuffer device is hard-coded to `/dev/fb0` at the moment because I'm lazy and it's half-past 3 in the morning.

Look, if anything is broken, just know I've only tested this on my Raspberry Pi 4, so feel free to add an issue or something; Or even better: fix it and send a PR.

## What does this do?

Good question.

## How do I compile it?
`gcc src/bitmap.c src/charmap.c src/gfx.c src/draw.c -Iinclude -o draw` should do the trick. Feel free to add optimization flags or something...

Also do `gcc src/bitmap.c src/bmconv.c -Iinclude -o bmconv` for the bitmap converter.

## What is bmconv?
So... I made a bitmap format for creating characters because I can't be bothered to look up existing formats. It only accepts 1's or 0's, but that's kind of a mess to write as bits in a file, so I just made a human-readable version of the format (with the extension `.rbm`). Bitmaps must be rectangular delimited by newline characters after each row (look at the existing files in `res`). To convert these human-readable files to a more compressed format, you use `bmconv c {input file} {output file}`. To decompress compressed files, run `bmconv d {input file} {output file}`.

Oh right, the bitmap format uses the `.bm` extension. If there are any issues, `bitmap.c` natively supports both the compressed and human-readable formats, so there's an upshot, I guess.

I forgot to add this before I commited: `res/maps.meta` describes all available characters and their corresponding bitmap files. Once again, I was lazy, so the format is as follows:
1. Each line describes a character mapping
2. The first character of each line describes which character is being mapped
3. The rest of the line describes the file path (relative to the directory containing the `meta.maps` file) of the character bitmap file

If you want to get the bitmap of a given character, just look at the main function in `src/draw.c` for a reference. I can't be bothered to explain it here. Sorry.

## Why??
I don't know. I just want to sleep

## Features?
* It doesn't crash (I think)
* Automatic character mapping from a metadata file
* Double-buffered frames
* Character transparency
* Bitmaps are actual bits ;)

## TODO
* Obviously fix the language in this README. This is waaaaaaay too informal
* Take a nap
* Proper bounds-detection for things being drawn (instead of just crashing or wrapping)
* Check for memory leaks
* Scale-able characters
* More characters
* Full-colour characters
* Input events system
* Support for triangles :)
* Rotation for elements on-screen
* Un-hard-code the framebuffer device
* ???
* Profit

