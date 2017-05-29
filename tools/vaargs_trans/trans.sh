
echo "use $0 <file-list>"


cat $1  | while read line
do
    echo $line $line.src
    ./vaargs_trans < $line  > $line.src
    mv $line.src $line
done

