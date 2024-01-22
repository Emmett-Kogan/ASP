make

./mapper <testfiles/$1 >mapper_out.txt
./reducer <mapper_out.txt >reducer_out.txt
./combiner <testfiles/$1 >combiner_out.txt

diff reducer_out.txt combiner_out.txt

if [[ $? -ne 0 ]]
then
    echo "Combiner output is not the same as output with redirection operator"
fi

rm mapper_out.txt reducer_out.txt combiner_out.txt
make clean
