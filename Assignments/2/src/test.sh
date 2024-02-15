# Author: Emmett Kogan
# Last modified: 2/11/24

users=(3 5 10 10 20)
depths=(1 2 3 5 10 20 $RANDOM $RANDOM $RANDOM $RANDOM $RANDOM)

make

# This tests each with a FIFO depth of 1, I also need to test for a bunch of different depths
# Also, not in the spec is a number of FIFOs less than the number of users, but it doesn't specify that it can't be more. Mine handles this edgecase by just not using a bunch, but it may need to do something better for less

for i in {0..4}; do
    for j in ${depths[@]}; do
        ./combiner $j ${users[$i]} <testfiles/input$i.txt | sort > temp.txt
        diff temp.txt testfiles/output$i.txt >/dev/null
        if [[ $? -ne 0 ]]; then
            echo "Differences found on test $i with depth $j"
        fi
    done
done

make clean
rm temp.txt
