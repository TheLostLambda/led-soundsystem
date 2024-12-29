watch:
    watchexec -e c,h just run

run: build
    picotool load -f build/*.uf2

build:
    cd build; make

configure:
    mkdir build -p
    cd build; cmake ..

clean:
    rm -r build

monitor:
    minicom -o -D /dev/ttyACM0

reconfigure: clean configure
