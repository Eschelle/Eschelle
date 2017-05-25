# Eschelle - WIP

## Example

To see an example look [here](Example.esch).

## Compiling

To compile Eschelle you need:

```
cmake, make, & 1 of (clang, gcc, vsc++)
```

Then cd into the repository directory and run the following:

```
mkdir build
(cd build && cmake .. && make)
```

After you build Eschelle you can run it with:

```
./Eschelle /path/to/esch/code
```

Example:

```
./Eschelle ../Example.esch
```