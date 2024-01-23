make

./combiner <testfiles/in/0.txt >0.txt
diff 0.txt testfiles/out/0.txt

./combiner <testfiles/in/1.txt >1.txt
diff 1.txt testfiles/out/1.txt

./combiner <testfiles/in/2.txt >2.txt
diff 2.txt testfiles/out/2.txt

./combiner <testfiles/in/3.txt >3.txt
diff 3.txt testfiles/out/3.txt

./combiner <testfiles/in/4.txt >4.txt
diff 4.txt testfiles/out/4.txt

make clean
