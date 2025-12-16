// Function example for EDA Dialect
// Demonstrates function definition and calling

module {
  // Define a function that processes a file
  "eda.func"(@process_file, (!tcl_string) -> i32) {
  ^bb0(%filename: !tcl_string):
    // Check if file exists
    %exists = "eda.file_exists"(%filename) : (!tcl_string) -> i1

    %result = "eda.if"(%exists) ({
      // File exists, read and process it
      "eda.puts"("Processing file:") : () -> ()
      "eda.puts"(%filename) : (!tcl_string) -> ()

      %content = "eda.file_read"(%filename) : (!tcl_string) -> !tcl_string
      "eda.puts"("File content:") : () -> ()
      "eda.puts"(%content) : (!tcl_string) -> ()

      "eda.return"(1) : (i32) -> i32
    }, {
      // File doesn't exist
      "eda.puts"("File not found:") : () -> ()
      "eda.puts"(%filename) : (!tcl_string) -> ()
      "eda.return"(0) : (i32) -> i32
    }) : (i32) -> i32
  }

  // Define a function that finds log files
  "eda.func"(@find_log_files, (!tcl_string) -> !tcl.list<!tcl.string>) {
  ^bb0(%directory: !tcl_string):
    // Create glob pattern for log files
    %pattern = "eda.concat"(%directory, "/*.log") : (!tcl_string, !tcl_string) -> !tcl_string

    // Find matching files
    %files = "eda.file_glob"(%pattern) : (!tcl_string) -> !tcl.list<!tcl.string>

    "eda.return"(%files) : (!tcl.list<!tcl.string>) -> !tcl.list<!tcl.string>
  }

  // Define a utility function for formatting messages
  "eda.func"(@format_message, (!tcl_string, i32) -> !tcl.string) {
  ^bb0(%level: !tcl_string, %value: i32):
    %formatted = "eda.format"("[%s] Value: %d", %level, %value) : (!tcl_string, !tcl_string, i32) -> !tcl_string
    "eda.return"(%formatted) : (!tcl_string) -> !tcl_string
  }

  // Main execution
  // Initialize some variables
  "eda.store"("log_directory", "/var/log") : (!tcl_string) -> ()
  "eda.store"("process_count", 0) : (i32) -> ()

  // Call find_log_files function
  %log_dir = "eda.load"("log_directory") : () -> !tcl_string
  %log_files = "eda.call"(@find_log_files, %log_dir) : (!tcl_string) -> !tcl.list<!tcl.string>

  // Process each log file (simplified)
  "eda.puts"("Found log files:") : () -> ()
  "eda.puts"(%log_files) : (!tcl.list<!tcl.string>) -> ()

  // Call process_file with a test file
  %test_file = "eda.constant"("test.log") : !tcl_string
  %process_result = "eda.call"(@process_file, %test_file) : (!tcl_string) -> i32

  // Update and show process count
  %current_count = "eda.load"("process_count") : () -> i32
  %new_count = "eda.add"(%current_count, %process_result) : (i32, i32) -> i32
  "eda.store"("process_count", %new_count) : (i32) -> ()

  // Use the format_message utility
  %msg = "eda.call"(@format_message, "INFO", %new_count) : (!tcl_string, i32) -> !tcl_string
  "eda.puts"(%msg) : (!tcl_string) -> ()
}