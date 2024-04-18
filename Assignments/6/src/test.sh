for i in {1..4}; do
    sudo insmod Assignment6Code.ko

    make app
    echo "Running test $i"
    # run test, make sure to kill all of the children too
    sudo ./test $i & sleep 10 && pkill -P $!

    if [[ $? -eq 0 ]]; then
        echo "Timed out after 10s, most likely because of deadlock :)"
    fi
    sudo rmmod Assignment6Code
done