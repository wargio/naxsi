![naxsi](logo.png)

## Documentation

Updated documentation is available [here](https://wargio.github.io/naxsi/).

## What is Naxsi?

NAXSI means [Nginx](http://nginx.org/) Anti [XSS](https://www.owasp.org/index.php/Cross-site_Scripting_%28XSS%29) & [SQL Injection](https://www.owasp.org/index.php/SQL_injection). 

Technically, it is a third party nginx module, available as a package for
many UNIX-like platforms. This module, by default, reads a small subset of
simple (and readable) rules containing 99% of known patterns involved in
website vulnerabilities. For example, `<`, `|` or `drop` are not supposed
to be part of a URI.

Being very simple, those patterns may match legitimate queries, it is
the Naxsi's administrator duty to add specific rules that will whitelist
legitimate behaviours. The administrator can either add whitelists manually
by analyzing nginx's error log, or (recommended) start the project with an
intensive auto-learning phase that will automatically generate whitelisting
rules regarding a website's behaviour.

In short, Naxsi behaves like a DROP-by-default firewall, the only task
is to add required ACCEPT rules for the target website to work properly.

## Why is it different?

Contrary to most Web Application Firewalls, Naxsi doesn't rely on a
signature base like an antivirus, and thus cannot be circumvented by an
"unknown" attack pattern.
Naxsi is [Free software](https://www.gnu.org/licenses/gpl.html) (as in freedom)
and free (as in free beer) to use.

## What does it run on?
Naxsi should be compatible with any nginx version.

It depends on `libpcre` for its regexp support, and is reported to work great on NetBSD, FreeBSD, OpenBSD, Debian, Ubuntu and CentOS.

## Why using this repository

The [original project](https://github.com/nbs-system/naxsi) has been **officially archived** as of November 8th, 2023. However, you can still request support, as I remain the last active developer and am committed to maintaining it in this fork.

## Sponsors

Support this project by [becoming a sponsor](https://github.com/sponsors/wargio).

## Build naxsi

**Be sure when you clone the repository to fetch all the submodules.**

```
$ git clone --recurse-submodules https://github.com/wargio/naxsi.git
$ wget --no-clobber -O nginx.tar.gz "https://nginx.org/download/nginx-1.22.0.tar.gz"
$ mkdir nginx-source
$ tar -C nginx-source -xzf nginx.tar.gz --strip-components=1
$ cd nginx-source
$ ./configure  --prefix=/etc/nginx --add-dynamic-module=../naxsi/naxsi_src
$ make
```

# Support & Bugs

Questions & bug reports regarding NAXSI can be addressed via issues.

[Click here to open an issue](https://github.com/wargio/naxsi/issues/new)

# Vulnerability disclosure

When disclosing vulnerabilities, please send first an email to `deroad at kumo.xn--q9jyb4c` (gpg keyid: `29656E856786B9A1FBF983CFA219F52A8217B1FE`)

```
-----BEGIN PGP PUBLIC KEY BLOCK-----
mDMEXR3FZhYJKwYBBAHaRw8BAQdAfuSDE68TEppjJfUAApXSTsHrKtGefVJXvz7f
YIO0gci0MUdpb3Zhbm5pIERhbnRlIEdyYXppb2xpIDxkZXJvYWRAa3Vtby54bi0t
cTlqeWI0Yz6IkAQTFggAOAIbAwULCQgHAgYVCgkICwIEFgIDAQIeAQIXgBYhBCll
boVnhrmh+/mDz6IZ9SqCF7H+BQJg9+CGAAoJEKIZ9SqCF7H+X04BAPXz7R856z72
f8nsZZjj4q3YaJbXA3pSVLIJ9uDQniCsAP9PaPBcbr231M3cjjBMo7ovlrElfFor
zCWA3NhRb4Y2DLg4BF0dxWYSCisGAQQBl1UBBQEBB0AVby+EIQcIoqSDZelvkqt8
dV1kvF4f/J0jj0k3OEKNcAMBCAeIeAQYFggAIAIbDBYhBCllboVnhrmh+/mDz6IZ
9SqCF7H+BQJg9+C4AAoJEKIZ9SqCF7H+ZfQBAOFb7iZm7t8j5ymiXyJFcuM/nF9+
bx4+KJJUTR9r6zBFAQD3Ea5Ya4lny/v9WKNSpBfZFEs3pkDnfgxw3o8vc4iSAQ==
=d/IR
-----END PGP PUBLIC KEY BLOCK-----
```
