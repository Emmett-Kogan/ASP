# Author: Emmett Kogan
# Last modified: 2/11/24

users=(3 5 10 10 20)
depths=(1 2 3 5 10 20 $RANDOM $RANDOM $RANDOM $RANDOM $RANDOM)

make

# This tests each with a FIFO depth of 1, I also need to test for a bunch of different depths
# Also, not in the spec is a number of FIFOs less than the number of users, but it doesn't specify that it can't be more. Mine handles this edgecase by just not using a bunch, but it may need to do something better for less

for i in {0..4}; do
    for j in ${depths[@]}; do

        # want to shuffle the input file
        shuf testfiles/input$i.txt >shuffled.txt

        ./combiner $j ${users[$i]} <shuffled.txt > temp.txt

        if [[ $? -ne 0 ]]; then
            echo "Error occured"
        fi

        sort temp.txt > s.txt
        diff s.txt testfiles/output$i.txt >/dev/null

        if [[ $? -ne 0 ]]; then
            cat temp.txt
            echo "Differences found on test $i with depth $j"
        fi
    done
done

make clean
rm temp.txt s.txt shuffled.txt
