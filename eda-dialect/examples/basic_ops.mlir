// Basic operations example for EDA Dialect
// Demonstrates variable operations and basic arithmetic

module {
  // Variable assignments
  %timeout = "eda.assign"("timeout", 10) : (i32) -> i32
  %debug_mode = "eda.assign"("debug_mode", true) : (i1) -> i1
  %log_level = "eda.assign"("log_level", "info") : (!tcl_string) -> !tcl_string

  // Load variables
  %timeout_val = "eda.load"("timeout") : () -> i32
  %debug_val = "eda.load"("debug_mode") : () -> i1

  // Arithmetic operations
  %const_5 = "eda.constant"(5) : i32
  %sum = "eda.add"(%timeout_val, %const_5) : (i32, i32) -> i32
  %doubled = "eda.mul"(%timeout_val, 2) : (i32, i32) -> i32

  // Comparison operations
  %is_timeout_gt_15 = "eda.cmp_gt"(%timeout_val, 15) : (i32, i32) -> i1
  %is_debug_enabled = "eda.cmp_eq"(%debug_val, true) : (i1, i1) -> i1

  // Logical operations
  %should_log = "eda.and"(%is_timeout_gt_15, %is_debug_enabled) : (i1, i1) -> i1

  // String operations
  %timeout_str = "eda.format"("%d", %timeout_val) : (!tcl_string, i32) -> !tcl_string
  %message = "eda.concat"("Timeout value: ", %timeout_str) : (!tcl_string, !tcl_string) -> !tcl_string

  // System operations
  "eda.puts"(%message) : (!tcl_string) -> ()
  "eda.puts"("Debug mode:", %debug_val) : (!tcl_string, i1) -> ()

  // Store new values
  "eda.store"("timeout", 20) : (i32) -> ()
  "eda.store"("log_level", "warning") : (!tcl_string) -> ()
}