`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Author:         lsilvest
//
// Create Date:    02/03/2008 
//
// Module Name:    lcd_write_number
//
// Target Devices: Spartan 3E starter kit Rev D
//
// Tool versions:  Tested with ISE WebPack 9.2
//
// Description:    This module writes a 32-bit hexadecimal number to the LCD
//                 display character by character using the lcd display module.
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

module lcd_write_number
  (
   input  CLK_50MHZ,
   output LCD_E,
   output LCD_RS,
   output LCD_RW,
   output [3:0] LCD_D,

   input  [31:0] if_data,
   input         if_write,
   output        if_ready
   );
  
  reg [7:0]  disp_data;
  reg        disp_rs;
  reg [31:0] disp_delay;
  reg        disp_write;
  wire       disp_ready;
  reg        disp_b8;
  
  reg [7:0]  char;
  reg [1:0]  state;
  reg [31:0] number;
  reg        init_done;
  reg        running;
  reg [4:0]  shift_cntr;
  
  reg        if_ready_r;
  assign if_ready = if_ready_r;

  
  lcd_display display
    (.clk(CLK_50MHZ),
     .rst(1'b0),

     .lcd_e(LCD_E),
     .lcd_rw(LCD_RW),
     .lcd_rs(LCD_RS),
     .lcd_d(LCD_D),

     .if_data(disp_data),
     .if_rs(disp_rs),
     .if_delay(disp_delay),
     .if_write(disp_write),
     .if_ready(disp_ready),
     .if_8bit(disp_b8)
     );

  
  
  parameter NB_CHARS     = 8'd12;
  
  parameter START        = 2'b00,
            WAIT_WRITE_0 = 2'b01,
            WRITE_1      = 2'b10,
            WAIT_WRITE_1 = 2'b11;


  initial begin
    state <= 2'b00;
    char <= 8'b0;
    init_done <= 1'b0;
    if_ready_r <= 1'b0;
    shift_cntr <= 5'b0;
  end

  
  always@ (posedge CLK_50MHZ) begin
    if (init_done && char > 8'd16) begin
      if (disp_ready)
        if_ready_r <= 1'b1;
      if (if_write) begin
        char <= 4'd8;          // reset the display
      end
    end else if (char <= 8'd16) begin
      if_ready_r <= 1'b0;
      
      case (state)
        START:
          if (disp_ready) begin
            disp_write <= 1'b1;
            state <= WAIT_WRITE_0;
          end
        
        WAIT_WRITE_0:
          state <= WRITE_1;
        
        WRITE_1:
          begin
            disp_write <= 1'b0;
            state <= 2'b11;
          end
        
        WAIT_WRITE_1:
          begin
            state <= START;
            char <= char + 8'b1;
          end 
      endcase // case (state)
    end // else: !if(!running)
  end // always@ (posedge CLK_50MHZ)


  always@ (negedge CLK_50MHZ) begin
    // these next steps initialize the LCD display:
    case (char)
      0: 
        begin 
          disp_b8 <= 1'b0;
          disp_data <= 8'h30;
          disp_delay <= 32'd10000000;
          disp_rs <= 1'b0;
        end

      1: disp_data <= 8'h30;

      2: 
        begin 
          disp_data <= 8'h30;
          disp_delay <= 32'd1000000;
        end

      3: 
        begin 
          disp_data <= 8'h20;
          disp_delay <= 32'd20000;
        end
      
      4: 
        begin 
          disp_b8 <= 1'b1;
          disp_data <= 8'h28;
        end
      
      5: disp_data <= 8'h06;
      
      6: disp_data <= 8'h0C;

      7: 
        begin 
          disp_data <= 8'h01;
          disp_delay <= 32'd1000000;
          init_done <= 1'b1;
          shift_cntr <= 5'd9;
        end

      8: // this state provides an entry point to reset the display and then
        // go on to the default state that writes the number
        begin 
          disp_rs <= 1'b0;
          disp_data <= 8'h01;
          disp_delay <= 32'd1000000;
          shift_cntr <= 5'b0;
          number <= if_data;
        end
      
      default:
        // state machine to print a 32-bit number out
        if (disp_ready && state == START) begin
          if (shift_cntr < 5'd8) begin
            disp_rs <= 1'b1;
            disp_delay <= 32'd20000;
            if (number[31:28] < 4'b1010)
              disp_data <= number[31:28] + 8'h30;
            else
              disp_data <= number[31:28] + 8'h37;
            number <= number << 4;
            shift_cntr <= shift_cntr + 5'b1;
          end 
        end
    endcase // case (char)  
  end // always@ (negedge CLK_50MHZ)
  
endmodule
