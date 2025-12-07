# amethyst

2d Renderer Library made for ReCraft3DS based on palladium.

## Usage

### Submodule:

Add amethyst as submodule to your repository

```bash
git submodule add https://github.com/tobid7/amethyst vendor/amethyst
```

Add it as submodule to your `CMakeLists.txt`

```cmake
add_subdirectory(vendor/amethyst)
target_link_libraries(${PROJECT_NAME} PUBLIC amethyst)
```

### Installed

Coming Soon...

## Credits

- [tobid7](https://github.com/tobid7) Lead Developer, Stealing a lot of code of [Palladium](https://github.com/tobid7/palladium)
- [nothings](https://github.com/nothings) stb_image and stb_truetype from [here](https://github.com/nothings/stb)