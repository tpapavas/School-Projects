`timescale 1ns/1ns

module adder #(parameter N=8) (
  input logic [N-1:0] a, b,
  input logic cin,
  output logic [N-1:0] s,
  output logic cout);
  
  assign {cout, s} = a + b + cin;
  
endmodule
  
module subtractor #(parameter N=8) (
  input logic [N-1:0] a, b,
  output logic [N-1:0] y);
  
  assign y = a - b;
  
endmodule

module andN #(parameter N=8) (
  input logic [N-1:0] a, b,
  output logic [N-1:0] y);
  
  assign y = a & b;
  
endmodule

module orN #(parameter N=8) (
  input logic [N-1:0] a, b,
  output logic [N-1:0] y);
  
  assign y = a | b;
  
endmodule

module ALU #(parameter N=8) (
  input logic [N-1:0] a, b,
  input logic [1:0] c,
  output logic [N-1:0] y,
  output logic cout,
  output logic [3:0] flags);
  
  // flags => oVerflow-Carry-Negative-Zero (VCNZ)
  // control signal c => 00 (add), 01 (sub), 10 (AND), 11 (OR)
  
  logic [N-1:0] s0, s1, s2, s3;
  
  adder #(N) alu_adder(a, b, c[0], s0, cout);
  subtractor #(N) alu_sub(a, b, s1);
  andN #(N) alu_andN(a, b, s2);
  orN #(N) alu_orN(a, b, s3);
  
  assign y = c[1] ? (c[0] ? s3 : s2) : (c[0] ? s1 : s0);
  
  assign flags[3] = ~c[1] & (a[N-1] ^ y[N-1]) & ~(c[0] ^ a[N-1] ^ b[N-1]); // has oVerflow
  assign flags[2] = ~c[1] & cout; // has Carry
  assign flags[1] = y[N-1]; // is Negative
  assign flags[0] = ~(|y); // is Zero
  
endmodule