watch:
    watchexec -e c,h just run

reboot:
    picotool reboot -f

run: build
    picotool load -f build/*.uf2

build:
    cd build; make

configure:
    mkdir build -p
    cd build; cmake ..

clean:
    rm -rf build

monitor:
    minicom -o -D /dev/ttyACM0

reconfigure: clean configure
