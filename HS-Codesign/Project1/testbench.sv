`timescale 1ns/1ns

`define N 4
`define TV_WIDTH 4*`N + 2

module testbench();
  logic clk, reset;
  logic [`N-1:0] a, b, y, yexpected;
  logic [1:0] c;
  logic cout;
  logic [3:0] flags, flagsexpected;
  logic [31:0] vectornum, errors;
  logic [`TV_WIDTH-1:0] testvectors[0:5];
  
  logic [`TV_WIDTH-1:0] undefinedvector = `TV_WIDTH'bx;
  
  //instantiate device under test
  ALU #(4) alu(a, b, c, y, cout, flags);
  
  //generate clock
  always #5 clk = ~clk;
  
  //at start of test, load vectors
  initial
    begin
      $dumpfile("dump.vcd");
      $dumpvars(1, testbench);
      
      $readmemb("alu.tv", testvectors);

      clk = 1'b0;
      vectornum=0; errors=0;
      reset=0;
    end
 
  //apply test vectors on rising edge of clk
  always @(posedge clk)
    begin
      #1;{a, b, c, yexpected, flagsexpected} = testvectors[vectornum];
      
      #1 $display("inputs = a=%b, b=%b, c=%b", a, b, c);
    end
 
  // check results on falling edge of clk
  always @(negedge clk)
    if(~reset) begin //skip during reset
      if(y !== yexpected) begin //check result
        $display("Error (Output): inputs = a=%b, b=%b, c=%b", a, b, c);
        $display("outputs = %b (%b expected)", y, yexpected);
        errors = errors + 1;
      end
      if(flags !== flagsexpected) begin //check flags
        $display("Error (Flags): inputs = a=%b, b=%b, c=%b", a, b, c);
        $display("flags = %b (%b expected)", flags, flagsexpected);
        errors = errors + 1;
      end
      vectornum = vectornum + 1;
      if(testvectors[vectornum] === undefinedvector) begin
        $display("%d tests completed with %d errors",
                 vectornum, errors);
        $finish;
      end
    end
 endmodule