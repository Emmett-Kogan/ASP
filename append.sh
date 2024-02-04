# Quick script to append name and modification data to the top of source files
# Provide a relative path to the target directory in arg1 and the date in arg2

for file in $1/*; do
	echo "$file"

	if [[ "$file" == *.c || "$file" == *.h || "$file" == *.cpp ]]; then
		echo "// Author: Emmett Kogan" >> t.txt
		echo "// Last modified: $2" >> t.txt
		echo >> t.txt
		cat $file >> t.txt
		mv t.txt $file
	elif [[ "$file" == *Makefile ]]; then
		echo "# Author: Emmett Kogan" >> t.txt
		echo "# Last modified: $2" >> t.txt
		echo >> t.txt
		cat $file >> t.txt
		mv t.txt $file
	fi
	
done
