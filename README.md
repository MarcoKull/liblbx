# liblbx
A simple C++ library to read and write lbx files -
a container format used in some DOS games developed by Simtex:

- Master of Magic

- Master of Orion

- Master of Orion 2

- 1830: Railroads & Robber Barons

It is intended for developers that want to be able to view or alter lbx files.
Some files in these games have a .LBX suffix but aren't real lbx file containers. This also seems to be true for all lbx files contained in StarLords - a precessor of Master of Orion.

## Getting started

To build, test and install the library, using cmake command line, a typical workflow is:

1. Get the source code and change to it.
```bash
git clone https://github.com/MarcoKull/liblbx.git
cd liblbx
```

2. Create a build directory and change to it.
```bash
mkdir build
cd build
```

3. Run cmake to configure the build tree.
```bash
cmake -DBUILD_SHARED_LIBS=ON -G "Unix Makefiles" ..
```

4. Compile the project.
```bash
cmake --build .
```

5. Run tests.
```bash
ctest .
```

6. Install header and compiled library.
```bash
make install
```

Now you should be able to use the library in your application:
```
#include <lbx.h>
#include <iostream>

int main(int argc, char** argv) {
    try {
        LbxFile lbx(argv[1]);
        std::cout << "Archive " << argv[1] << " contains " << lbx.size() << " files:\n";
        for (int i = 0; i < lbx.size(); ++i) {
            std::cout << " " << i + 1 << ":\t" << lbx[i].size << " byte\n";
        }
        return 0;
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
}
```

## First steps
To get familiar with the API see the simple [class diagram](./doc/class-diagram.svg):

![class diagram](./doc/class-diagram.svg)

If you are intrested any further have a look at the [file format description](./doc/lbx-format.pdf).
There are still some unclear aspects, maybe you have an idea?