# Keylog

A stealthy Linux keylogger that hides itself from `lsmod` and `/proc/modules`.

## Build
Make sure you have `linux-headers` installed.

```console
$ make
```

## Development
A Vagrant box with essential tools/packages is provided. Simply run the
following commands to build the box:

```console
$ vagrant up
$ vagrant ssh
vagrant@stretch:~$ cd /vagrant
```
