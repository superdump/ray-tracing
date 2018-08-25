# Ray Tracing in One Weekend

This project is my personal step through of [Peter Shirley's Ray Tracing in One Weekend](http://in1weekend.blogspot.com/2016/01/ray-tracing-in-one-weekend.html). I may (have) continue(d) on from the end of those books to add some experiments of my own.

## What is here?

Currently I have implemented:
* Ray Tracing in One Weekend
* Ray Tracing: the Next Week
* Multi-threading using [enkiTS](https://github.com/dougbinks/enkiTS) which is [zlib-licensed](https://github.com/dougbinks/enkiTS/blob/master/License.txt) and included in-tree
* XorShift PRNG with per-thread state as in Aras' work which is using [The Unlicense](https://github.com/aras-p/ToyPathTracer/blob/master/license.md).
* A framebuffer preview using [MiniFB](https://github.com/emoon/minifb) by Daniel Collin which is [MIT-licensed](https://github.com/emoon/minifb/blob/master/LICENSE) and included in-tree
* Image reading/writing using the [single-file 'stb' image libraries](https://github.com/nothings/stb) which are [public-domain- or MIT- licensed](https://github.com/nothings/stb#whats-the-license)
* A [NASA Blue Marble Next Generation image](https://visibleearth.nasa.gov/view.php?id=73909) for texturing the Earth spheres
    * R. Stöckli, E. Vermote, N. Saleous, R. Simmon and D. Herring (2005). The Blue Marble Next Generation - A true color earth dataset including seasonal dynamics from MODIS. Published by the NASA Earth Observatory. Corresponding author: rstockli@climate.gsfc.nasa.gov
* [docopt.cpp](https://github.com/docopt/docopt.cpp) for super convenient command line parsing which is [MIT-licensed](https://github.com/docopt/docopt.cpp/blob/master/LICENSE-MIT)

## Usage

I have used the [meson build system](https://mesonbuild.com/) for the project as I find writing portable `Makefile`s more pain than I'm willing on my spare time, if I can avoid it. :)

### (Optional) Update Code

```
git pull --rebase
# enkiTS and minifb are in git submodules
git submodule update --init --recursive
```

### Building

**NOTE: Don't forget to init/update the submodules!**
```
meson builddir
cd builddir
ninja
```

### Running

From within `builddir`:
```
./rtiow
```

Help:
```
./rtiow --help
Ray Tracing in One Week.

    Usage:
      rtiow [-s SCENE] [-w WIDTH] [-h HEIGHT] [-r RAYS_PER_PIXEL]
            [-b MAX_BOUNCES_PER_RAY] [-t TEXTURE] [-o OUTPUT]
      rtiow (-h | --help)
      rtiow --version

    Options:
      --help                Show this screen.
      --version             Show version.
      -w, --width=WIDTH     Width of output image. [default: 800]
      -h, --height=HEIGHT   Height of output image. [default: 800]
      -s, --scene=SCENE     Name of scene to be rendered. One of:
                            earth, two_spheres, final, cornell_final,
                            cornell_balls cornell_smoke cornell_box,
                            two_perlin_spheres simple_light random_scene
                            [default: final]
      -o, --output=OUTPUT   Output PNG file path. [default: image.png]
      -r, --rays-per-pixel=RAYS_PER_PIXEL Number of rays to cast per pixel.
                            [default: 100]
      -b, --bounces-per-ray=MAX_BOUNCES_PER_RAY Maximum number of bounces per
                            ray. [default: 50]
      -t, --texture=TEXTURE Relative path to a JPEG texture.
                            [default: ../data/world.topo.bathy.200412.3x5400x2700.jpg]
```

## Peter Shirley's Books

Peter Shirley has very kindly made the series of books publicly available. There are kindle versions available on Amazon or grab the PDFs via Shirley's page above for the price of "pay what you wish" with 50% going to not for profit programming education organisations. See the page above for details! :)

There are also code repositories for each of the books:
* [Ray Tracing in One Weekend](https://github.com/petershirley/raytracinginoneweekend)
* [Ray Tracing the Next Week](https://github.com/petershirley/raytracingthenextweek)
* [Ray Tracing the Rest of Your Life](https://github.com/petershirley/raytracingtherestofyourlife)

## Optimisations

While implementing the path tracing ray tracer, I started to wonder about potential optimisations. It seems path tracers and in particular Shirley's Ray Tracing in One Weekend series is hot right now and others have already done the work.

I was interested in exploring an implementation in Rust to capitalise on the language's thread safety through memory safety. [Cameron Hart](https://github.com/bitshifter) (a game developer) made a set of interesting blog posts about writing an optimised Rust implementation:
1. [Rust ray tracer in one weekend](https://bitshifter.github.io/2018/04/29/rust-ray-tracer-in-one-weekend/)
2. [Path tracing in parallel with Rayon](https://bitshifter.github.io/2018/05/07/path-tracing-in-parallel/)
3. [Optimising path tracing with SIMD](https://bitshifter.github.io/2018/06/04/simd-path-tracing/)
4. [Optimising path tracing: the last 10%](https://bitshifter.github.io/2018/06/20/the-last-10-percent/)

Hart's material referred to an [optimisation-focused blog series](http://aras-p.info/blog/2018/03/28/Daily-Pathtracer-Part-0-Intro/) by [Aras Pranckevičius](https://github.com/aras-p) (a Unity developer) that is highly worth reading. It also includes Unity burst-compiled and GPU-optimised variants that achieve really impressive performance! [Code here](https://github.com/aras-p/ToyPathTracer).

## License

As so much of the code is from Peter Shirley's public domain code, with some modifications similar to the 'Unlicense'-licensed public domain code by Aras Pranckevičius, it is only fitting that I follow suit and make this code public domain using [The Unlicense](https://unlicense.org/). See the LICENSE file for details.
