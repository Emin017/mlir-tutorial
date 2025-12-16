// Simple example that demonstrates basic EDA Dialect operations
// This can be translated to TCL using mlir-eda-translate

module {
  // Variable assignments
  "eda.assign"("timeout", 10) : (i32) -> i32
  "eda.assign"("debug_mode", true) : (i1) -> i1
  "eda.assign"("log_level", "info") : (!tcl_string) -> !tcl_string

  // Simple message output
  "eda.puts"("Hello, EDA World!") : () -> ()

  // Simple arithmetic
  %const_5 = "eda.constant"(5) : i32
  %sum = "eda.add"(%const_5, 3) : (i32, i32) -> i32

  // Store the result
  "eda.store"("result", %sum) : (i32) -> ()

  // Simple conditional
  %is_gt_10 = "eda.cmp_gt"(%sum, 10) : (i32, i32) -> i1
  "eda.if"(%is_gt_10) ({
    "eda.puts"("Sum is greater than 10") : () -> ()
  }) : () -> ()

  // Simple loop
  %start = "eda.constant"(0) : i32
  %end = "eda.constant"(3) : i32
  "eda.for"(%start, %end) ({
  ^bb0(%i: i32):
    %i_str = "eda.format"("Loop iteration: %d", %i) : (!tcl_string, i32) -> !tcl_string
    "eda.puts"(%i_str) : (!tcl_string) -> ()
    "eda.yield"() : () -> ()
  }) : () -> ()
}