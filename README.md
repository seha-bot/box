# Seha utils

Cross-platform build environment for C.

## Setup

Clone the repository or use it as a template and just run make!

```bash
  git clone https://github.com/seha-bot/seha-utils.git
  cd seha-utils && make
```

### Dependencies

Create a `deps.list` file in the root of your project and put your dependencies as download links.

deps.list example:
```
mysource.net/header.h
mysource.net/source.c
```

Do not prepend http or https to your links as it is currently an issue!

These dependencies will automatically be managed and you can include them anywhere in your code!

**Note:** Adding new dependencies is as simple as adding a new line to deps.list, but when removing, you have to clean your project `make clean`.

### Includes folder

Make a folder called `inc` and put your header files in there. You can include them everywhere in your code except inside dependencies. It makes no sense to add them there!

### Libraries

This is still a work in progress as I'm still making it possible to automatically handle every platform

