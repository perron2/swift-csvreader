// import Foundation

public class CsvReader {
    var delimiter: Character = ","
    var textQualifier = "\""
    var trimWhitespace = true
    var skipEmptyRecords = true
    private(set) var numFields = 0

    init?(path: String) {
        self.path = path
        guard let csv = csv_init(path) else {
            return nil
        }

        csv.pointee.delimiter = Int8(String(delimiter).utf8.first!)
        csv.pointee.trim_whitespace = trimWhitespace
        csv.pointee.skip_empty_records = skipEmptyRecords
        self.csv = csv
    }

    deinit {
        csv_free(csv)
    }

    func readHeaders() -> Bool {
        if !readRecord() {
            return false
        }

        headers = [String: Int](minimumCapacity: numFields)
        for i in 0..<numFields {
            let header = String(cString: csv_get_field(csv, Int32(i)))
            headers[header] = i
        }
        return true
    }

    func readRecord() -> Bool {
        if csv_read_record(csv) {
            numFields = Int(csv_get_field_count(csv))
            return true
        } else {
            numFields = 0
            return false
        }
    }

    func index(_ name: String) -> Int {
        return headers[name] ?? -1
    }

    func string(_ index: Int) -> String {
        if let str = String(validatingUTF8: csv_get_field(csv, Int32(index))) {
            return str
        }
        return ""
    }

    func string(_ name: String) -> String {
        return string(index(name))
    }

    func stringOrNil(_ index: Int) -> String? {
        let value = string(index)
        return value == "" ? nil : value
    }

    func stringOrNil(_ name: String) -> String? {
        let value = string(name)
        return value == "" ? nil : value
    }

    private var path: String
    private var csv: UnsafeMutablePointer<csv_parser>
    private var headers = [String: Int]()
}
