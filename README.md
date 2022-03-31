# How to build on Linux

``` sh
export SCITER_SDK_HOME=<path-to-sciter-js-sdk> # i.e ~/projects/sciter-js-sdk
export QT_HOME=<path-to-qt-install-dir> # i.e. /opt/Qt/5.12.9/gcc_64
mkdir build
cd build
ln -s ${SCITER_SDK_HOME}/bin.lite/linux/x64/libsciter.so .
cmake .. -DSCITER_SDK_HOME=${SCITER_SDK_HOME} -DCMAKE_PREFIX_PATH:STRING=${QT_HOME}
cmake --build .
```
Or you can simply import the `CMakeLists.txt` on QtCreator IDE