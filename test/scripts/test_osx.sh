set -x

brew install cmake
brew install pyenv

pyenv install 3.8.0
pyenv global 3.8.0
eval "$( pyenv init - )"
pip install python-dateutil

git clean -dfx
git submodule init
git submodule update
cmake -DCMAKE_BUILD_TYPE=debug .
make -j2
make install

pushd test || exit
make
./run_all -v
grep 'not ok' all.log
set +x
./problems
echo "timew $( timew --version )"
python --version
python3 --version
cmake --version
clang --version