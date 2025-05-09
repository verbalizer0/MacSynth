import os

project_root = os.path.dirname(os.path.abspath(__file__))  # assumes script is in project root
file_extensions = [".cpp", ".h"]

for root, dirs, files in os.walk(project_root):
    for file in files:
        if any(file.endswith(ext) for ext in file_extensions):
            full_path = os.path.join(root, file)
            relative_path = os.path.relpath(full_path, project_root)

            with open(full_path, "r") as f:
                lines = f.readlines()

            # Remove existing file path comment if already present
            if lines and lines[0].startswith("// File:"):
                lines.pop(0)

            # Insert the new comment
            lines.insert(0, f"// File: {relative_path}\n")

            with open(full_path, "w") as f:
                f.writelines(lines)

print("File path comments added.")
