import Foundation

public class CsvReader {
    var delimiter: Character = ","
    var textQualifier = "\""
    var trimWhitespace: Bool = true
    var skipEmptyRecords: Bool = true
    private(set) var numFields: Int = 0

    init?(path: String) {
        self.path = path
        csv = csv_init(path)
        if csv == nil {
            return nil
        }

        csv.memory.delimiter = Int8(String(delimiter).utf8.first!)
        csv.memory.delimiter = Int8(String(delimiter).utf8.first!)
        csv.memory.trim_whitespace = trimWhitespace
        csv.memory.skip_empty_records = skipEmptyRecords
    }

    deinit {
        csv_free(csv)
    }

    func readHeaders() -> Bool {
        if !readRecord() {
            return false
        }

        headers = [String:Int](minimumCapacity: numFields)
        for i in 0..<numFields {
            let header = String.fromCString(csv_get_field(csv, Int32(i))) ?? ""
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

    func getIndex(name: String) -> Int {
        return headers[name] ?? -1
    }

    func get(index: Int) -> String {
        if let str = String.fromCString(csv_get_field(csv, Int32(index))) {
            return str
        }
        return ""
    }

    func get(name: String) -> String {
        return get(getIndex(name))
    }

    private var path: String
    private var csv: UnsafeMutablePointer<csv_parser>
    private var headers = [String:Int]()
}