#ifndef DCTOOLSWRAPPER_H
#define DCTOOLSWRAPPER_H
#include <libdivecomputer/context.h>
#include <libdivecomputer/device.h>
#include <libdivecomputer/descriptor.h>
#include <libdivecomputer/iostream.h>
#include <libdivecomputer/serial.h>
#include <iostream>


class DCWrapper
{
public:
    /** Constructeur
     */
    DCWrapper();
    ~DCWrapper();

    bool connect(const std::string& vendor,
                 const std::string& product);

    void disconnect();

    bool isConnected() const;

private:
    dc_context_t* context = nullptr;
    dc_iostream_t* iostream = nullptr;
    dc_device_t* device = nullptr;
    dc_descriptor_t* descriptor = nullptr;

    bool openSerial();
    bool findDescriptor(const std::string& vendor,
                        const std::string& product);
};

#endif // DCTOOLSWRAPPER_H
