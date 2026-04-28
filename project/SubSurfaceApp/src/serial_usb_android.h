#ifndef SERIAL_USB_ANDROID_H
#define SERIAL_USB_ANDROID_H

#include <string>
#include <vector>
#include <QJniObject>

/* USB Device Information */
struct android_usb_serial_device_descriptor {
    QJniObject usbDevice; /* the UsbDevice */
    std::string className; /* the driver class name. If empty, then "autodetect" */
    std::string uiRepresentation; /* The string that can be used for the user interface. */

    // Device information
    std::string usbManufacturer;
    std::string usbProduct;
    std::string manufacturer;
    std::string product;
    uint16_t pid;
    uint16_t vid;
};

std::vector<android_usb_serial_device_descriptor> serial_usb_android_get_devices();
android_usb_serial_device_descriptor getDescriptor(QJniObject usbDevice);

// --- FONCTIONS À AJOUTER POUR L'EXPOSITION ---

// Ouvre le flux avec le descripteur complet (celle que tu utiliseras dans le wrapper)
dc_status_t serial_usb_android_open(dc_iostream_t **iostream, dc_context_t *context, void *androidUsbDevice);

// Version spécifique utilisée en interne (optionnelle dans le .h, mais utile si besoin)
dc_status_t serial_usb_android_open(dc_iostream_t **iostream, dc_context_t *context, QJniObject usbDevice, std::string driverClassName);

#endif