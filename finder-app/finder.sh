#!/bin/sh







# Verify that both the directory path and search string arguments are present, both arguments must be present
if [ $# -lt 2 ]; then
    echo "Error: Two arguments are  required: <filesdir> <searchstr>"
    exit 1
fi

filesdir=$1
searchstr=$2

# Confirms if the path represents a valid directory
if [ ! -d "$filesdir" ]; then   #checks the path (directory using -d)
    echo "Error: $filesdir is not a valid directory"
    exit 1
fi

# Counts the number of files in the directory
X=$(find "$filesdir" -type f | wc -l)

# Counts total number of lines across all files that contain the search string
Y=$(grep -r "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $X and the number of matching lines are $Y"
