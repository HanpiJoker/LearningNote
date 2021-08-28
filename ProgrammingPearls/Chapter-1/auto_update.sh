#/bin/bash

if [[ -f *.txt ]]; then
	rm *.txt
fi

if [[ -f "sortbitmap" || -f "sortlib" || "create" ]]; then
	rm sortbitmap sortlib create
fi

# compile function
gcc rand.c -o create
if [[ $? != 0 ]]; then
	exit -1
fi

gcc sort_lib.c -o sortlib
if [[ $? != 0 ]]; then
	exit -1
fi

gcc sort_bitmap.c -o sortbitmap
if [[ $? != 0 ]]; then
	exit -1
fi


echo "Run command: create to create data need sort"
./create
echo "Run command: sortlib to sort data with library function"
./sortlib
echo "Run command: sortbitmap to sort data with bitmap"
./sortbitmap
