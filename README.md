# morpho-curl

Morpho extension that provides a high-level interface to the [libcurl](https://curl.se/libcurl/) network transfer library. You can use this interface to fetch data from a URL in just a couple of lines of morpho code:

    import curl 
    var data = Curl("www.google.com").fetch()

## Installation

To install this, ensure you have zeromq installed using:

    brew install libcurl [macOS]
    apt get libcurl-dev  [ubuntu/WSL]

Then clone this repository onto your computer in any convenient place:

    git clone https://github.com/morpho-lang/morpho-curl.git

then add the location of this repository to your .morphopackages file.

    echo PACKAGEPATH >> ~/.morphopackages 
    where PACKAGEPATH is the location of the git repository.

You need to compile the extension, which you can do by cd'ing to the repository's base folder and typing

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. 
    make install

The package can then be loaded into morpho using the `import` keyword.

    import curl
