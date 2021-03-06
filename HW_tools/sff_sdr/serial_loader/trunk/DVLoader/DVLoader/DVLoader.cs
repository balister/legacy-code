/****************************************************************
 *  TI DVEVM Serial Application Loader                     
 *  (C) 2006, Texas Instruments, Inc.                           
 *                                                              
 * Author:  Daniel Allred (d-allred@ti.com)                     
 * History: 11/16/2006 - Initial release
 *                                                              
 ****************************************************************/

using System;
using System.Text;
using System.IO;
using System.IO.Ports;

namespace DVLoader
{   
    //Main program class
    class Program
    {
        /// <summary>
        /// Main entry point of application
        /// </summary>
        /// <param name="args">Array of command-line arguments</param>
        /// <returns>Return code: 0 for correct exit, -1 for unexpected exit</returns>
        static Int32 Main(String[] args)
        {
            SerialPort MySP;
            string SPName;
            string appFileName;
            string startAddr;
        
            // Begin main code
            Console.WriteLine("-----------------------------------------------------");
            Console.WriteLine("   TI DVEVM Serial Application Loader                ");
            Console.WriteLine("   (C) 2006, Texas Instruments, Inc.                 ");
            Console.WriteLine("-----------------------------------------------------");
            Console.Write("\n\n");

            // Check that a filename was passed on the command-line
            if ((args.Length < 1) || (args.Length >2))
            {
                Console.WriteLine("Usage: DVSerial [startAddress] <app binary file>");
                return -1;
            }
            else
            {
                if (args.Length == 1)
                {
                    startAddr = "0100";
                    appFileName = args[0];
                }
                else
                {
                    startAddr = args[0];
                    appFileName = args[1];
                }
            }
            

            // Determine correct serial port device name (depends on platform)
            int p = (int)Environment.OSVersion.Platform;
            if ((p == 4) || (p == 128)) //Check for unix
                SPName = "/dev/ttyS0";
            else
                SPName = "COM1";
            
            // Attempt to open serial port
            try
            {
                Console.WriteLine("Attempting to connect to device " + SPName + "...");
                MySP = new SerialPort(SPName, 115200, Parity.None, 8, StopBits.One);
                MySP.Encoding = Encoding.ASCII;
                MySP.ReadTimeout = 500;
                MySP.Open();
            }
            catch(Exception e)
            {
                if (e is UnauthorizedAccessException)
                {
                    Console.WriteLine(e.Message);
                    Console.WriteLine("This application failed to open the serial port.");
                    Console.WriteLine("Most likely it is in use by some other application.");
                }
                else
                {
                    Console.WriteLine(e.Message);
                }
                return -1;
            }

            // Send the application file
            TransmitSerialApp(appFileName, startAddr, MySP);

            // Close the serial port
            MySP.Close();
            
            // Return success
            return 0;    
            
        }

        /// <summary>
        /// Function to Transmit the UBL via the DM644x ROM Serial boot
        /// </summary>
        private static void TransmitSerialApp(String appFileName, String startAddr, SerialPort serPort)
        {
            // Local Variables for reading UBL file
            FileStream appStream;
            BinaryReader appBR;
            StringBuilder appSB;
            Byte[] fileData;
            UInt32 CRCValue;
            UInt32 data;
            CRC32 MyCRC;
            
            // Open File
            appStream = File.OpenRead(appFileName);
            
            // Setup the binary stream reader
            appBR = new BinaryReader(appStream);
            
            // Create the byte array and stringbuilder for holding UBL file data
            fileData = new Byte[appStream.Length];
            appSB = new StringBuilder(fileData.Length * 2);

            // Read the data from the app file into the appropriate structures
            for (int i = 0; i < (appStream.Length / sizeof(UInt32)); i++)
            {
                data = appBR.ReadUInt32();
                appSB.AppendFormat("{0:X8}", data);

                System.BitConverter.GetBytes(data).CopyTo(fileData, i * sizeof(UInt32));
            }

            // DM644x expects the bitwise inverse of standard CRC-32 algorithm
            MyCRC = new CRC32();
            CRCValue = MyCRC.CalculateCRC(fileData);
            CRCValue = ~CRCValue;

            try
            {
            BOOTMESEQ:
                Console.WriteLine("\nWaiting for DVEVM...");

                // Wait for the DVEVM to send the ^BOOTME/0 sequence
                if (ReadSeq(" BOOTME\0", serPort))
                    Console.WriteLine("BOOTME commmand received. Returning ACK and header...");
                else
                    goto BOOTMESEQ;

                // Output 28 Bytes for the ACK sequence and header
                // 8 bytes acknowledge sequence = "    ACK\0"
                serPort.Write("    ACK\0");
                // 8 bytes of CRC data = ASCII string of 8 hex characters
                serPort.Write(CRCValue.ToString("X8"));
                // 4 bytes of UBL data size = ASCII string of 4 hex characters (3800h = 14336d)
                serPort.Write(appStream.Length.ToString("X4"));
                // 4 bytes of start address = ASCII string of 4 hex characters (>=0100h)
                serPort.Write(startAddr);
                // 4 bytes of constant zeros = "0000"
                serPort.Write("0000");
                Console.WriteLine("ACK command sent. Waiting for BEGIN command... ");

                // Wait for the BEGIN sequence
                if (ReadSeq("  BEGIN\0", serPort))
                    Console.WriteLine("BEGIN commmand received. Sending CRC table...");
                else
                    goto BOOTMESEQ;

                // Send the 1024 byte (256 word) CRC table
                for (int i = 0; i < MyCRC.Length; i++)
                {
                    serPort.Write(MyCRC[i].ToString("x8"));
                }
                Console.WriteLine("CRC table sent.  Waiting for DONE...");
                

                // Wait for the first DONE sequence
                if (ReadSeq("   DONE\0", serPort))
                    Console.WriteLine("DONE received.  Sending the Application file...");
                else
                    goto BOOTMESEQ;

                // Send the contents of the UBL file 
                serPort.Write(appSB.ToString());

                // Wait for the second DONE sequence
                if (ReadSeq("   DONE\0", serPort))
                    Console.WriteLine("DONE received.  Application file was accepted.");
                else
                    goto BOOTMESEQ;

                Console.WriteLine("Application transmitted successfully.\n");

            }
            catch (ObjectDisposedException e)
            {
                Console.WriteLine(e.StackTrace);
                throw e;
            }
        }

        /// <summary>
        /// Check for a certain sequence from the serial port.
        /// </summary>
        /// <param name="testStr">String to look for.</param>
        /// <param name="sp">The serialport object to use.</param>
        /// <returns>True if string detected, false if not detected or timeout.</returns>
        private static bool ReadSeq(string testStr, SerialPort sp)
        {
            int numChar = testStr.Length;

            Byte[] input = new Byte[numChar];

            for (int i = 0; i < numChar; i++)
            {
                try
                {
                    input[i] = (Byte)sp.ReadByte();
                    //Console.Write((Char)input[i]);
                }
                catch (TimeoutException)
                {
                    Console.WriteLine("Serial port timeout");
                    return false;
                }
            }
            string inputStr = (new ASCIIEncoding()).GetString(input, 0, numChar);

            return String.Equals(inputStr, testStr);
        }

    } // end class program

    // CRC-32 class
    public class CRC32
    {
        #region Data members

        private UInt32[] lut;
        private UInt32 poly; //Bit32 is 1 is always 1 and therefore not needed
        private UInt32 initReg;
        private UInt32 finalReg;

        #endregion

        #region Indexer and Properties

        public UInt32 this[int index]
        {
            get
            {
                return lut[index];
            }
        }

        public Int32 Length
        {
            get
            {
                return lut.Length;
            }
        }

        #endregion

        /// <summary>
        /// Basic Constructor to generate the standard CRC-32 (used in ethernet packets, Zip files, etc.)
        /// </summary>
        public CRC32()
        {
            poly = 0x04C11DB7; //Bit32 is 1 by default
            initReg = 0xFFFFFFFF;
            finalReg = 0xFFFFFFFF;
            BuildTable();
        }

        /// <summary>
        /// Function to generate the table of values used in the CRC-32 calculation
        /// </summary>
        private void BuildTable()
        {
            Int32 NumBitsPerRegShift = 8;
            Int32 tableLen = (Int32)Math.Pow(2.0, NumBitsPerRegShift);
            UInt32 crcAccum;
            lut = new UInt32[tableLen];

            for (UInt32 i = 0; i < tableLen; i++)
            {
                crcAccum = ReflectNum(i, NumBitsPerRegShift) << (32 - NumBitsPerRegShift);
                for (Byte j = 0; j < NumBitsPerRegShift; j++)
                {
                    if ((crcAccum & 0x80000000) != 0x00000000)
                    {
                        crcAccum = (crcAccum << 1) ^ poly;
                    }
                    else
                    {
                        crcAccum = (crcAccum << 1);
                    }
                    lut[i] = ReflectNum(crcAccum, 32);
                }
            }
        }

        /// <summary>
        /// Calculate the CRC-32 checksum on the given array of bytes
        /// </summary>
        /// <param name="Data">Array of bytes of data.</param>
        /// <returns>The 32-bit CRC of the data.</returns>
        public UInt32 CalculateCRC(Byte[] Data)
        {
            UInt32 crc = initReg;
            Int32 len = Data.Length;
            Int32 NumBitsPerRegShift = 8;
            UInt32 Mask = (UInt32)(Math.Pow(2.0, NumBitsPerRegShift) - 1);

            // Perform the algorithm on each byte
            for (Int32 i = 0; i < len; i++)
            {
                crc = (crc >> NumBitsPerRegShift) ^ lut[(crc & Mask) ^ Data[i]];
            }
            // Exclusive OR the result with the specified value
            return (crc ^ finalReg);
        }

        /// <summary>
        /// Method to reflect a specified number of bits in the integer
        /// </summary>
        /// <param name="inVal">The unsigned integer value input</param>
        /// <param name="num">The number of lower bits to reflect.</param>
        /// <returns></returns>
        private UInt32 ReflectNum(UInt32 inVal, Int32 num)
        {
            UInt32 outVal = 0x0;

            for (Int32 i = 1; i < (num + 1); i++)
            {
                if ((inVal & 0x1) != 0x0)
                {
                    outVal |= (UInt32)(0x1 << (num - i));
                }
                inVal >>= 1;
            }
            return outVal;
        }

    } //end class CRC32

} // end namespace
