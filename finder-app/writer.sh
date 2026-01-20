#File Header - This is the writer.sh script, to create files and directories at the same time

# Attributions: looked up on comparison operators, and used https://www.freecodecamp.org/news/shell-scripting-crash-course-how-to-write-bash-scripts-in-linux/ as a reference for syntax



#!/bin/sh

# This is to verify if both the file path and text string arguments have been provided
if [ $# -lt 2 ]; then   # Here, $# is the argument counter variable
    echo "Error: The required arguments missing. Usage: $0 <writefile> <writestr>"
    exit 1
fi

writefile=$1
writestr=$2

# Creates the parent directory path if it does not already exist
mkdir -p "$(dirname "$writefile")"

# Writes the string to the file, overwriting any previous content
echo "$writestr" > "$writefile"

# Verify if the file was successfully created and written
if [ $? -ne 0 ]; then
    echo "Error: Could not create or write to $writefile"
    exit 1
fi
