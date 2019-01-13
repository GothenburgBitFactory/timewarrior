set -x

brew install gnutls
brew install cmake
brew install libfaketime
pip install dateutil
git clean -dfx
git submodule init
git submodule update
cmake -DCMAKE_BUILD_TYPE=debug .
make -j2
make install
timew --version

pushd test
make
./run_all -v
cat all.log | grep 'not ok'
./problems
