# Doc LibDC

## DC Tool
### Manuel
Accessible avec la commande 
```bash
$ dctool help
```

Retour : 
```
A simple command line interface for the libdivecomputer library

Usage:
   dctool [options] <command> [<args>]

Options:
   -h, --help                Show help message
   -d, --device <device>     Device name
   -f, --family <family>     Device family type
   -m, --model <model>       Device model number
   -l, --logfile <logfile>   Logfile
   -q, --quiet               Quiet mode
   -v, --verbose             Verbose mode

Available commands:
   help       Show basic help instructions
   version    Show version information
   list       List supported devices
   scan       Scan for supported devices
   download   Download the dives
   dump       Download a memory dump
   parse      Parse previously downloaded dives
   read       Read data from the internal memory
   write      Write data to the internal memory
   timesync   Synchronize the device clock
   fwupdate   Update the firmware

See 'dctool help <command>' for more information on a specific command.
```

### Télécharger les plongées d'un ordinateur

Commande : 
```bash
$ dctool -d <device_name> download -t serial -o <output_file_name> <serial_interface>
```
Variables à insérer :

* <device_name>  : Un appareil parmis ceux supportés. ("Mares Quad Air" par exemple)<br>
Liste des appareils supportés : 
```bash
$ dctool list
```
* <output_file_name> : nom du fichier xml qui sera créé dans le repertoire courant. (test.xml par exemple)
* <serial_interface> : pour conaître le port série de l'ordinateur de plongée : 
    1. brancher/connecter l'ordinateur de plongée.
    2. executer la commande: 
        ```bash 
        $ dctool scan -t serial
        ``` 

        sur un sytème UNIX, le retour doit ressembler à ça en fonction des appareils connectés.
        ```
        /dev/tty.BLTH
        /dev/tty.Bluetooth-Incoming-Port
        /dev/tty.usbserial-0001      <-- adresse de de mon Mares Quad Air
        ```
    3. le <serial_interface> est <code>/dev/tty.usbserial-0001</code>
    