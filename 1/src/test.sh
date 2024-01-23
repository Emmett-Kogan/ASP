# this script assumes that the executable files are produced with a makefile and that they are named combiner reducer and mapper respectivley
# this script also assumes you have a 'testfiles' directory that contains the files Yavuz distributed
# note that test case 0 is technically broken because the ordering is different, Yavuz said that this is fine as long as the lines match (I didn't feel like sorting anything so I just left it as is)

make

for i in {0..4}; do
    ./mapper <testfiles/input$i.txt >temp.txt
    ./reducer <temp.txt >manual.txt


    ./combiner <testfiles/input$i.txt >mp.txt
    sleep 1s

    diff manual.txt mp.txt
    if [[ $? -ne 0 ]]
    then
        echo "Manual redirection output does not match combiner output"
    fi

    diff mp.txt testfiles/output$i.txt
    if [[ $? -ne 0 ]]
    then
        echo "Combiner output does not match given output"
    fi
done

rm mp.txt manual.txt
make clean
