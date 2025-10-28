import os
import re

def find_definitions_in_headers(directory):
    """
    Finds and prints function and class method definitions in C/C++ header files
    within a specified directory.
    """
    header_extensions = ['.h', '.hpp', '.hxx']

    # Regex for function definitions (basic, might need refinement for complex cases)
    # Catches return_type function_name(arguments)
    function_pattern = re.compile(r'^\s*(?:[\w_]+\s+)+\w+\s*\([^)]*\)\s*[{;]', re.MULTILINE)

    # Regex for class method definitions (basic, might need refinement)
    # Catches return_type ClassName::method_name(arguments)
    method_pattern = re.compile(r'^\s*(?:[\w_]+\s+)+[\w_]+::\w+\s*\([^)]*\)\s*[{;]', re.MULTILINE)

    for root, _, files in os.walk(directory):
        for file in files:
            if any(file.endswith(ext) for ext in header_extensions):
                filepath = os.path.join(root, file)
                print(f"{filepath}")
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()

                        # Find function definitions
                        functions = function_pattern.findall(content)
                        if functions:
                            #print("Functions Found:")
                            for func in functions:
                                print(f"{func.strip()}")

                        # Find class method definitions
                        methods = method_pattern.findall(content)
                        if methods:
                            #print("Class Methods Found:")
                            for method in methods:
                                print(f"{method.strip()}")

                        if not functions and not methods:
                            print("No function or class method definitions found.")

                except Exception as e:
                    print(f"Error reading {filepath}: {e}")

# Example usage:
# Replace 'your_directory_path' with the actual path to your header files
target_directory = 'inc/'
find_definitions_in_headers(target_directory)
