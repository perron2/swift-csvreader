CsvReader
=========
CsvReader is a simple and fast CsvReader for Swift. For performance reasons the core functionality is implemented with C.

Example
-------
```swift
if let csv = CsvReader(path: "file.csv") {
    csv.readHeaders()
    while csv.readRecord() {
        print(csv.get("name"))
        print(csv.get("age"))
    }
}
```

For better performance you can determine the indexes of the columns before looping through the rows:

```swift
if let csv = CsvReader(path: "file.csv") {
    csv.readHeaders()
    let nameIndex = csv.getIndex("name")
    let ageIndex = csv.getIndex("age")
    while csv.readRecord() {
        print(csv.get(nameIndex))
        print(csv.get(ageIndex))
    }
}
```

Installation
------------
Add the files to your project and make sure you have a bridging header file that includes CsvReader.h:

```c
#ifndef Bridging_Header_h
#define Bridging_Header_h

#include "CsvReader/CsvReader.h"

#endif /* Bridging_Header_h */

```
