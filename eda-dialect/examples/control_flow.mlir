// Control flow example for EDA Dialect
// Demonstrates if/else and loop constructs

module {
  // Load a variable
  %timeout = "eda.load"("timeout") : () -> i32
  %retry_count = "eda.load"("retry_count") : () -> i32

  // If-then-else construct
  %cmp1 = "eda.cmp_gt"(%timeout, 15) : (i32, i32) -> i1
  %result = "eda.if"(%cmp1) ({
    // Then branch
    "eda.puts"("Timeout is too high!") : () -> ()
    "eda.store"("retry_count", 5) : (i32) -> ()
    "eda.yield"(true) : (i1) -> ()
  }, {
    // Else branch
    %cmp2 = "eda.cmp_eq"(%timeout, 10) : (i32, i32) -> i1
    %inner_result = "eda.if"(%cmp2) ({
      // Nested if
      "eda.puts"("Using default timeout") : () -> ()
      "eda.store"("retry_count", 3) : (i32) -> ()
      "eda.yield"(false) : (i1) -> ()
    }, {
      // Final else
      "eda.puts"("Using custom timeout") : () -> ()
      "eda.yield"(false) : (i1) -> ()
    }) : (i1) -> i1
    "eda.yield"(%inner_result) : (i1) -> ()
  }) : (i1) -> i1

  // For loop example
  %start = "eda.constant"(0) : i32
  %end = "eda.constant"(5) : i32
  "eda.for"(%start, %end) ({
  ^bb0(%i: i32):
    %i_str = "eda.format"("Iteration %d", %i) : (!tcl_string, i32) -> !tcl_string
    "eda.puts"(%i_str) : (!tcl_string) -> ()

    // Check condition inside loop
    %cmp_i = "eda.cmp_eq"(%i, 3) : (i32, i32) -> i1
    "eda.if"(%cmp_i) ({
      "eda.puts"("Reached iteration 3, continuing") : () -> ()
      "eda.continue"() : () -> ()
      "eda.yield"() : () -> ()
    }) : () -> ()

    "eda.yield"() : () -> ()
  }) : () -> ()

  // While loop example
  "eda.while"({
    // Condition region
    %current_retry = "eda.load"("retry_count") : () -> i32
    %retry_gt_0 = "eda.cmp_gt"(%current_retry, 0) : (i32, i32) -> i1
    "eda.yield"(%retry_gt_0) : (i1) -> ()
  }, {
    // Body region
    %current = "eda.load"("retry_count") : () -> i32
    %new_count = "eda.sub"(%current, 1) : (i32, i32) -> i32
    "eda.store"("retry_count", %new_count) : (i32) -> ()
    "eda.puts"("Retrying...") : () -> ()
    "eda.yield"() : () -> ()
  }) : () -> ()
}