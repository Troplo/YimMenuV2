# Navigate to the folder containing your .cpp files
cd src/game/features

# Find all .cpp files recursively
find . -type f -name "*.cpp" | while read file; do
    # Make a backup first
    #cp "$file" "$file.bak"

    # Prepend #if TOXIC_CHEATS and append #endif
    sed -i '1i#if TOXIC_CHEATS' "$file"
    echo -e '\n#endif' >> "$file"
done
