// Iterate from 

make
make app

# just leaving default args for the sake of not doing a whole lot
sudo insmod char_driver.ko

for i in {0..4}; do

    sudo ./devuserapp testfiles/input$i.txt >temp.txt
    diff testfiles/output$i.txt temp.txt

    if [[ $? -ne 0 ]]; then
        echo "Test case $i failed"
    fi

done

sudo rmmod char_driver
rm temp.txt
make clean
