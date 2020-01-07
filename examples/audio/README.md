# USB audio example

This is an example application implementing USB audio class (UAC) (see: [Universal Serial Bus Device Class Definition for Audio Devices](https://usb.org/)).

Device descriptor uses class/subclass/protocol compatible with Interface Association Descriptors
(see: [USB Interface Association Descriptor Device Class Code and Use Model, 1.0](https://www.usb.org/sites/default/files/iadclasscode_r10.pdf)).

There are two interfaces:

* Audio control interface: 0 endpoints - no need for interrupt endpoint

* Audio streaming interface (with alternate settings 0 and 1)
    * alternate setting 0: no endpoints, host can use it to disable audio in case of bandwidth problems
    * alternate setting 1: single isochronous endpoint

The application provides simple audio topology consisting of a single input terminal and single output terminal.
