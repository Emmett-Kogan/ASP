test_count=$(($(ls testfiles | wc -l)/2 -1))
make

thread_counts=(1,2,4,8,16,32)
for i in $(seq 0 $test_count); do
    for j in ${thread_counts[@]}; do
        timeout 20s ./a3 testfiles/input$i.txt $j >temp.txt
        if [[ $? -eq 124 ]]; then
            echo "Program timed out after 20 seconds during test $i with $j threads: possible deadlock"
            continue
        fi

        diff temp.txt testfiles/output0.txt >/dev/null
        if [[ $? -ne 0 ]]; then
            echo "Differences found during test $i with $j threads"
        fi
    done
done

make clean
rm temp.txt