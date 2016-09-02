// TODO: use LEDs for something
// TODO: CDC ACM control signals on GPIO pins

#include <xc.h>
#include <stdint.h>
#include <usb.h>
#include <usb_device.h>
#include <usb_device_cdc.h>
#include "usb_helpers.h"
#include "uart.h"

void usbToSerialService()
{
    // When we receive bytes on the UART's RX line, send them on the
    // virtual serial port.
    while (uartRxAvailable() && cdcTxAvailable())
    {
        cdcTxSendByte(uartRxReceiveByte());
    }

    // When we receive bytes on the virtual USB serial port, send them on
    // the UART's TX line.
    while (cdcRxAvailable() && uartTxAvailable())
    {
        uartTxSendByte(cdcRxReceiveByte());
    }
}

// When the computer sets the baud rate of the virtual USB serial port, we
// set the baud rate of the UART to match it.
void cdcSetBaudRateHandler()
{
    uartSetBaudRate(line_coding.dwDTERate);
}

void interrupt high_priority highIsr()
{
    USBDeviceTasks();
    uartIsr();
}

void interrupt low_priority lowIsr()
{
}

void main(void)
{
    uartInit();
    appUsbInit();

    while (1)
    {
        usbToSerialService();
        appUsbService();
    }
}

