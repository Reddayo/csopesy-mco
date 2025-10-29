import os
import re

def find_cpp_class_methods(directory):
    methods = {}
    method_pattern = re.compile(r'class\s+\w+\s*[^;]*?\{(.*?)\};', re.DOTALL)
    member_pattern = re.compile(r'^\s*(?:(?:virtual|static|inline|explicit|friend)\s+)?(?:[\w\d_<>:]+\s+(?:const\s+)?\*?\s*&?\s*|~?\w+)\s+\w+\s*\([^;]*\)\s*(?:const|noexcept)?\s*(&\s*)?;', re.MULTILINE)

    for filename in os.listdir(directory):
        if filename.endswith('.h'):
            filepath = os.path.join(directory, filename)
            try:
                with open(filepath, 'r') as f:
                    content = f.read()

                for class_match in method_pattern.finditer(content):
                    class_body = class_match.group(1)
                    file_methods = [m.group(0).strip() for m in member_pattern.finditer(class_body)]
                    if file_methods:
                        methods[filename] = file_methods

            except IOError:
                pass
    return methods

# Example usage (assuming 'include' is the directory with .h files)
class_methods = find_cpp_class_methods('inc/')
for file, m in class_methods.items():
    print(f"inc/{file}")
    for method in m:
        print(f"{method.strip()}")
