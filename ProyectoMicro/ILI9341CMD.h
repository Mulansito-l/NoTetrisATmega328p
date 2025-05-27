/*
 * ILI9341CMD.h
 *
 * Created: 03/05/2025 10:46:38 p. m.
 * Author: Mulansit
 *
 * This file contains the command definitions for the ILI9341 LCD controller.
 * These macro names are specifically chosen to match the glcd_init function
 * provided in GLCD.h for proper display initialization.
 */

#ifndef ILI9341CMD_H_
#define ILI9341CMD_H_

// --- System Function Commands ---
#define ILI9341_NOP                     0x00 // No Operation
#define ILI9341_SWRESET                 0x01 // Software Reset
#define ILI9341_RDID4                   0xD3 // Read ID4 (Read Display ID)

// --- Basic Display Control Commands ---
#define ILI9341_SLPOUT                  0x11 // Sleep Out
#define ILI9341_DISPOFF                 0x28 // Display OFF
#define ILI9341_DISPON                  0x29 // Display ON
#define ILI9341_MADCTL                  0x36 // Memory Access Control
#define ILI9341_COLMOD                  0x3A // Pixel Format Set (Color Mode)
#define ILI9341_RAMWR                   0x2C // Memory Write (Write to GRAM)
#define ILI9341_RAMRD                   0x2E // Memory Read (Read from GRAM)
#define ILI9341_CASET                   0x2A // Column Address Set
#define ILI9341_PASET                   0x2B // Page Address Set

// --- Power Control Commands ---
#define ILI9341_PWCTR1                  0xC0 // Power Control 1
#define ILI9341_PWCTR2                  0xC1 // Power Control 2
#define ILI9341_VMCTR1                  0xC5 // VCOM Control 1
#define ILI9341_VMCTR2                  0xC7 // VCOM Control 2
// The following are less common or have varying parameters, but included for completeness if your module uses them:
#define ILI9341_PWON_SEQ_CTL            0xCB // Power On Sequence Control
#define ILI9341_PCTR_A                  0xCD // Power Control A
#define ILI9341_PCTR_B                  0xCF // Power Control B

// --- Display Control Commands ---
#define ILI9341_FRMCTR1                 0xB1 // Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_DFUNCTR                 0xB6 // Display Function Control
#define ILI9341_INVCTR                  0xB4 // Display Inversion Control (general control)
#define ILI9341_DISP_INVERSION_OFF      0x20 // Display Inversion OFF (direct command)
#define ILI9341_DISP_INVERSION_ON       0x21 // Display Inversion ON (direct command)

// --- Gamma Control Commands ---
#define ILI9341_GAMSET                  0x26 // Gamma Set
#define ILI9341_GMCTRP1                 0xE0 // Positive Gamma Correction
#define ILI9341_GMCTRN1                 0xE1 // Negative Gamma Correction
#define ILI9341_3GAMMA_EN               0xF2 // Enable 3 Gamma Control

// --- Driver Timing Control Commands ---
#define ILI9341_DRVTIM_A                0xE8 // Driver Timing Control A
#define ILI9341_DRVTIM_B                0xEA // Driver Timing Control B

// --- Other Less Common/Specific Commands (for reference, may not be in glcd_init) ---
#define ILI9341_READ_DISP_STATUS        0x09 // Read Display Status
#define ILI9341_READ_DISP_POWER_MODE    0x0A // Read Display Power Mode
#define ILI9341_READ_DISP_MADCTL        0x0B // Read Display MADCTL
#define ILI9341_READ_DISP_PIXFMT        0x0C // Read Display Pixel Format
#define ILI9341_READ_DISP_IMG_FMT       0x0D // Read Display Image Format
#define ILI9341_READ_DISP_SIG_MODE      0x0E // Read Display Signal Mode
#define ILI9341_READ_DISP_SELF_DIAG     0x0F // Read Display Self-Diagnostic Result

#define ILI9341_PARTIAL_MODE            0x12 // Partial Mode ON
#define ILI9341_NORMAL_MODE             0x13 // Normal Display Mode ON

#define ILI9341_PARTIAL_AREA            0x30 // Partial Area
#define ILI9341_VSCRDEF                 0x33 // Vertical Scrolling Definition
#define ILI9341_TEAR_OFF                0x34 // Tearing Effect Line OFF
#define ILI9341_TEAR_ON                 0x35 // Tearing Effect Line ON
#define ILI9341_VSCRSADD                0x37 // Vertical Scrolling Start Address
#define ILI9341_IDLE_OFF                0x38 // Idle Mode OFF
#define ILI9341_IDLE_ON                 0x39 // Idle Mode ON

#define ILI9341_WRMEM_CONT              0x3C // Write Memory Continue
#define ILI9341_RDMEM_CONT              0x3E // Read Memory Continue

#define ILI9341_SET_TEAR_SCANLINE       0x44 // Set Tear Scanline
#define ILI9341_GET_SCANLINE            0x45 // Get Scanline

#define ILI9341_WRDISPB                 0x51 // Write Display Brightness
#define ILI9341_RDDISPB                 0x52 // Read Display Brightness
#define ILI9341_WRCTRLD                 0x53 // Write CTRL Display
#define ILI9341_RDCTRLD                 0x54 // Read CTRL Display
#define ILI9341_WRCABCM                 0x55 // Write Content Adaptive Brightness Control
#define ILI9341_RDCABCM                 0x56 // Read Content Adaptive Brightness Control
#define ILI9341_WRMINCAB                0x5E // Write CABC Minimum Brightness
#define ILI9341_RDMINCAB                0x5F // Read CABC Minimum Brightness

#define ILI9341_RGB_SIG_CTL             0xB0 // RGB Signal Control
#define ILI9341_FRMCTR2                 0xB2 // Frame Rate Control (Idle 8 Colors)
#define ILI9341_FRMCTR3                 0xB3 // Frame Rate Control (Partial Mode)
#define ILI9341_BLNKPORCH_CTL           0xB5 // Blanking Porch Control
#define ILI9341_ENTRY_MODE_SET          0xB7 // Entry Mode Set
#define ILI9341_BK_CTL1                 0xB8 // Backlight Control 1
#define ILI9341_BK_CTL2                 0xB9 // Backlight Control 2
#define ILI9341_BK_CTL3                 0xBA // Backlight Control 3
#define ILI9341_BK_CTL4                 0xBB // Backlight Control 4
#define ILI9341_BK_CTL5                 0xBC // Backlight Control 5
#define ILI9341_BK_CTL6                 0xBD // Backlight Control 6
#define ILI9341_BK_CTL7                 0xBE // Backlight Control 7
#define ILI9341_BK_CTL8                 0xBF // Backlight Control 8

#define ILI9341_NVMEM_WR                0xD0 // Non-Volatile Memory Write
#define ILI9341_NVMEM_PROT_KEY          0xD1 // Non-Volatile Memory Protection Key
#define ILI9431_NVMEM_STATUS            0xD2 // Non-Volatile Memory Status Read (Typo: ILI9431 corrected to ILI9341 if applicable)

#define ILI9341_DGAMCTL1                0xE2 // Digital Gamma Control 1
#define ILI9341_DGAMCTL2                0xE3 // Digital Gamma Control 2

#define ILI9341_INTERFACE_CTL           0xF6 // Interface Control
#define ILI9341_PUMP_RATIO_CTL          0xF7 // Pump Ratio Control

#endif /* ILI9341CMD_H_ */