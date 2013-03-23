for a in *.in ; do
    ./edmonds < "$a" | ./graf-dot | neato -Tpng > "$a".png
done
