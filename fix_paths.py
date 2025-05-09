import os

SRC_DIR = "src"
OLD_PREFIX = "bin/data/"
NEW_PREFIX = "data/"

for root, dirs, files in os.walk(SRC_DIR):
    for filename in files:
        if filename.endswith(".cpp") or filename.endswith(".h"):
            path = os.path.join(root, filename)
            with open(path, 'r') as f:
                content = f.read()
            if OLD_PREFIX in content:
                new_content = content.replace(OLD_PREFIX, NEW_PREFIX)
                with open(path, 'w') as f:
                    f.write(new_content)
                print(f"✔ Updated paths in: {path}")
