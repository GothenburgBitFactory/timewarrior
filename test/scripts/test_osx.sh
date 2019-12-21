set -x

brew install cmake
brew install python3
python3 -m pip install python3-dateutil
git clean -dfx
git submodule init
git submodule update
cmake -DCMAKE_BUILD_TYPE=debug .
make -j2
make install
timew --version

pushd test || exit
make
./run_all -v
grep 'not ok' all.log
./problems
