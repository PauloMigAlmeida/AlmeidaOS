# AlmeidaOS
My journey into learning how to create a hobbyist x86-64 simple OS... it will be in the works for years..don't expect anything ready


## Build
In order to build this app, you are expected to have `make` and `docker` installed in your machine. All of the other building dependencies should be taken care of automatically by the docker image created during this step.

To build it you have to run:

```{shell}
make build
```

## Run
In order to run this app, you are expected to have `make` and `qemu-system-x86_64` installed in your machine.

```{shell}
make test
```
