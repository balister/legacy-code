`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Engineer: lsilvest
//
// Create Date:   02/03/2008
//
// Module Name:   lcd_write_number_test
//
// Target Devices: Spartan 3E starter kit Rev D
//
// Tool versions:  Tested with ISE WebPack 9.2
//
//
// Description: This module tests the lcd_write_number module. It writes a counter
//              on the LCD display at each half second.
//
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

module lcd_write_number_test 
  (
   input  CLK_50MHZ,
   output LCD_E,
   output LCD_RS,
   output LCD_RW,
   output [3:0] LCD_D
   );
  
  wire if_ready;
  reg  if_write;
  reg [31:0] if_data;
  reg [1:0]  state;
  reg [31:0] cntr;

  parameter IDLE           = 2'b00,
            IF_WRITE_1     = 2'b01,
            SET_IF_WRITE_0 = 2'b10,
            WAIT           = 2'b11;


  // Instantiate the Unit Under Test (UUT)
  lcd_write_number uut 
    (
     .CLK_50MHZ(CLK_50MHZ), 
     .LCD_E(LCD_E), 
     .LCD_RS(LCD_RS), 
     .LCD_RW(LCD_RW), 
     .LCD_D(LCD_D), 
     .if_data(if_data), 
     .if_write(if_write), 
     .if_ready(if_ready)
     );


  initial begin
    if_data <= 32'habba0123;
    state <= IDLE;
    if_write <= 1'b0;
    cntr <= 32'b0;
  end


  always@ (posedge CLK_50MHZ) begin
    case (state)
      IDLE:
        if (if_ready) begin
          if_data <= if_data + 1'b1;
          if_write <= 1'b1;
          state <= IF_WRITE_1;
          cntr <= 32'b0;
        end
      
      IF_WRITE_1:            // this state to keep if_write up for 2 cycles
        state <= SET_IF_WRITE_0;       
      
      SET_IF_WRITE_0:        // set if_write 0 and start the counter 
        begin
          if_write <= 1'b0;
          state <= WAIT;
          cntr <= 32'b0;
        end
      
      WAIT:
        if (cntr < 25000000) // wait for 0.5 seconds
          cntr <= cntr + 32'b1;  
        else
          state <= IDLE;          
    endcase
  end
  
  
endmodule
