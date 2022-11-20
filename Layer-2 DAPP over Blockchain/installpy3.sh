#!/bin/bash

sudo add-apt-repository ppa:jonathonf/python-3.6 -y
sudo apt update -y
sudo apt install python3.6 -y
sudo apt install python3.6-dev -y
sudo apt install python3.6-venv -y
wget https://bootstrap.pypa.io/get-pip.py
sudo python3.6 get-pip.py
sudo ln -s /usr/bin/python3.6 /usr/local/bin/python3
sudo ln -s /usr/local/bin/pip /usr/local/bin/pip3
sudo pip3 install web3==4.9.0
sudo pip3 install py-solc

cd $HOME/HW3

git clone --recursive https://github.com/ethereum/solidity.git
cd solidity
git checkout v0.4.25
git submodule update --init --recursive
sh scripts/install_deps.sh
sudo sh scripts/build.sh


