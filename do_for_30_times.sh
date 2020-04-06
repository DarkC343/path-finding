# TIMEFORMAT=%R
# for i in {1..30}; do /usr/bin/time -f %E -o output.txt -a ./main -i map.bin -D -t 0 run -p; done

if $2 = dfs
then
    for i in {1..$1}; do /usr/bin/time -f %E -o $4 -a $5 -i $3 -D run -p; done
fi

elif $2 = bfs
then
    for i in {1..$1}; do /usr/bin/time -f %E -o $4 -a $5 -i $3 -B run -p; done
fi