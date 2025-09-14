# Creates a C++ map that maps letters to ASCII art

FONT_HEIGHT = 6

letters = []
lines = []

with open("text/plain.txt", "r") as file:
    letters = file.readlines()

letters = [x.strip() for x in letters]

with open("text/ascii.txt", "r") as file:
    lines = file.readlines()

grouped_lines = []

for i in range(0, len(lines), FONT_HEIGHT):
    chunk = lines[i:i + FONT_HEIGHT]

    chunk = ['"' + # Start with opening quotes
             x.replace('\\', '\\\\')      # Escape \ with \\ (esc. seq.)
              .replace('"', '\\"')        # Escape " with \"
              .replace("\n", "") + '",\n' # Precede newlines with a ",
            for x in chunk]
        
    # Join the elements in the chunk into a single string
    combined_string = "".join(chunk)
    grouped_lines.append(combined_string)

map = dict(zip(letters, grouped_lines))

for key, value in map.items():
    print(f"{{'{key}', {{\n{value}}}}},")