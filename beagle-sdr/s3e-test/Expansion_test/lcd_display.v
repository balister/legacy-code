`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Author: lsilvest
// 
// Create Date:    02/02/2008 
//
// Module Name:    lcd
// 
// Target Devices: Spartan 3E starter kit Rev D
//                 LCD controller part should be ST7066U but couldn't get it to
//                 work using timing and init sequence
//                 Xilinx says the LCD part is Okaya Electric America:
//                 RC1602ARS-DSO-A
//                 
//                 Timings used come from Xilinx design examples for the picoblaze
//                 KCPSM3
//
// Tool versions:  Tested with ISE WebPack 9.2
//
// Description: This LCD module provides the capability to write
//              a command or a character to the display. In particular
//              it presents an 8-bit interface and takes care of the
//              conversion to the 4-bit LCD interface.  To do so it
//              writes separately the upper and lower nibble and
//              generates the correct LCD_E pulse.
//
//              This module makes it easy to generate the init
//              sequence and various writing to the LCD display. Note
//              that it does not handle reads.
//
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
/////////////////////////////////////////////////////////////////////////////////

module lcd_display
  (input clk,
   input rst,

   output lcd_e,
   output lcd_rw,
   output lcd_rs,
   output [3:0] lcd_d,

   input [7:0] if_data,
   input if_rs,
   input [31:0] if_delay,
   input if_write,
   output if_ready,
   input if_8bit);

  reg [2:0] state;
  reg       lcdr_e;
  reg [3:0] lcdr_d;
  reg [31:0] wait_cntr;
  reg        ready;     
  reg        init_done;
  
  parameter IDLE              = 3'b000,
            WAIT_PULSE_E_0    = 3'b001,
            LOAD_LOWER_NIBBLE = 3'b010,
            WAIT_PULSE_E_1    = 3'b011,
            WAIT_COMMAND      = 3'b100;    

  parameter PULSE_E_DLY = 32'd12;
  parameter INIT_TIME   = 32'd20000000;
  
  assign lcd_d = lcdr_d;
  assign lcd_rs = if_rs;
  assign lcd_rw = 1'b0;
  assign lcd_e = lcdr_e;
  assign if_ready = ready;


  initial begin
    state <= IDLE;
    ready <= 1'b0;
    lcdr_e <= 1'b0;
    init_done <= 1'b0;
  end

  
  always@ (posedge clk) begin
    if (rst) begin
      state <= IDLE;
    end else if (!init_done) begin
      if (wait_cntr < INIT_TIME)
        wait_cntr <= wait_cntr + 1;
      else begin
        init_done <= 1'b1;
        ready <= 1'b1;
      end
    end else begin
      case (state)
        IDLE:
          begin
            if (if_write) begin
              lcdr_e <= 1'b1;
              lcdr_d <= if_data[7:4];  // upper nibble first
              ready <= 1'b0;
              wait_cntr <= 32'b0;
              state <= WAIT_PULSE_E_0;
            end
          end
        
        WAIT_PULSE_E_0:
          if (wait_cntr < PULSE_E_DLY) begin
            wait_cntr <= wait_cntr + 1;
          end else begin
            lcdr_e <= 1'b0;
            wait_cntr <= 0;
            if (if_8bit)
              state <= LOAD_LOWER_NIBBLE;
            else 
              state <= WAIT_COMMAND;
          end  
        
        LOAD_LOWER_NIBBLE:
          if (wait_cntr < PULSE_E_DLY) begin
            wait_cntr <= wait_cntr + 1;
          end else begin
            wait_cntr <= 0;
            lcdr_e <= 1'b1;
            lcdr_d <= if_data[3:0];  // lower nibble
            state <= WAIT_PULSE_E_1;
          end  
        
        WAIT_PULSE_E_1:
          if (wait_cntr < PULSE_E_DLY) begin
            wait_cntr <= wait_cntr + 1;
          end else begin
            lcdr_e <= 1'b0;
            wait_cntr <= 0;
            state <= WAIT_COMMAND;      
          end
        
        WAIT_COMMAND:
          if (wait_cntr < if_delay) begin
            wait_cntr <= wait_cntr + 32'b1;
          end else begin
            wait_cntr <= 0;
            if (!if_write) begin
              state <= IDLE;
              ready <= 1'b1;
            end
          end
      endcase // case (state)
    end
  end
  
endmodule
