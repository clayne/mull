#!/usr/bin/bash

set -e
set -x

git config --global --add safe.directory '*'
git submodule update --init --recursive

pip3 install -r requirements.txt

./infrastructure/generator.py cmake --os $1 --llvm_version $2

./end2end-tests/setup_end2end_tests.sh
