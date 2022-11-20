# python3 peersGraph.py
mkdir out
mv *.py out
mv *.txt out
name1="out/blockChainOn${1}.py"
name2="out/blockChainOn${2}.py"
python3 $name1
python3 $name2
mv *.svg out