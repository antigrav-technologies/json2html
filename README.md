# json2html (UNDER CONSTRUCTION YET)
Utility to convert JSON data to HTML

## Usage
```
json2html <json_file> <output_file>
```
## Description
A command-line utility that takes a JSON file as input and generates an HTML file as output. The JSON file should contain a valid JSON object that represents the structure of the HTML document.

## Input File Example
```json
{
    "html": {
        "head": {
            "title": {
                "$": "test page"
            }
        },
        "body": {
            "p": "Hello, world!"
        }
    }
}
```
This input file would generate an HTML file with a `<title>` element containing the text "test page" and a `<p>` element containing the text "Hello, world!".\
This _probably_ can be used as general purpose json to xml convertor but was designed to be for html.

## Notes
* The "body" object contains one or more elements (e.g. "p", "div", etc.) with a single key "$" containing the inner element value.
* If value is string then it will be the inner element value.
* Inner element values can also be dictionaries/objects
* Arrays are not supported.

Made as an experiment, any tweaks in code are welcome