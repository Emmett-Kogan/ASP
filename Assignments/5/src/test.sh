sizes=(0 11 0 52 0)

make >/dev/null 2>/dev/null
make app >/dev/null 2>/dev/null

for i in {0..4}; do
    if [[ ${sizes[$i]} -ne 0 ]]; then
        sudo insmod char_driver.ko size=${sizes[$i]}
    else
        sudo insmod char_driver.ko
    fi

    sudo ./devuserapp testfiles/input$i.txt >temp.txt 2>/dev/null
    diff testfiles/output$i.txt temp.txt

    if [[ $? -ne 0 ]]; then
        echo "Test case $i failed"
    fi

    sudo rmmod char_driver
done

rm temp.txt
make clean
