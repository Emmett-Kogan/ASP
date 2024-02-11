# Author: Emmett Kogan
# Last modified: 2/11/24

clear
make

./combiner 10 3 <testfiles/input0.txt | sort > temp.txt
diff temp.txt testfiles/output0.txt

make clean
rm temp.txt
