# DirectoryTemplates

Saving & loading file directory trees w/ file content variable substitution.

Lets you save any file/directory, where you can store template-specific variables.

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

See `template (load/save) --help` for help.

Ex:
```
# file.txt
Hello, ${name}! Your age is ${age}.
```
```bash
template save -i file.txt -o file.json
```
Now to load the file again...
```bash
template load -i file.json -o file.txt -k "name=\"Sarah\",age=\"18\""
```
```
# file.txt
Hello, Sarah! Your age is 18.
```

Works recursively through directories as well.

## Todo

* Fix -s and -e options.
* Allow variable substitution in file path names themselves.
* Generating statuc shell scripts that create the given file trees.