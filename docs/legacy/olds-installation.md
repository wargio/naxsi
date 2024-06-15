You can install nginx+naxsi either from packages (available from official repositories on debian, freebsd, netbsd) or directly from source.
As Nginx does not yet support runtime module loading, lot of people will choose compiling from source to avoid package maintainers delay.

## Installation From Packages

Packages are available for NetBSD, FreeBSD and Debian, from distribution's repositories.
Naxsi project does not create/publish such packages.

## Installation From Source

Naxsi should be working with all Nginx versions superior to 0.8.X.
To install it from source, we need to fetch both nginx and naxsi sources.

#### Import Source Signing Keys
Import [Nginx signing keys]( http://nginx.org/en/pgp_keys.html ) and [naxsi's one](https://pgp.mit.edu/pks/lookup?op=get&search=0x251A28DE2685AED4).

_note_ : Signed releases of naxsi started at [0.54rc3]( https://github.com/nbs-system/naxsi/tree/0.54rc3 ).

```shell
gpg --keyserver pgp.mit.edu --recv-keys 0x251A28DE2685AED4 7BD9BF62 A1C052F8
```

#### Download Sources
Note: replace `TAG_NAME` with the [tagged release](https://github.com/nbs-system/naxsi/tags) you want.

```shell
wget http://nginx.org/download/nginx-x.x.xx.tar.gz
wget http://nginx.org/download/nginx-x.x.xx.tar.gz.asc
wget https://github.com/nbs-system/naxsi/archive/TAG_NAME.tar.gz
wget https://github.com/nbs-system/naxsi/releases/download/TAG_NAME/TAG_NAME.tar.gz.asc
```

#### Verify Sources

```shell
gpg --verify nginx-*.tar.gz.asc nginx-*.tar.gz
gpg --verify TAG_NAME.tar.gz.asc TAG_NAME.tar.gz
```

Both source verifications should present you a message that says:

```
> gpg: Good signature from ...
```

#### Unpack Sources
If both sources verify, then it is safe to unpack and proceed to compilation.

```shell
tar -xvzf nginx-*.tar.gz
tar -xvzf TAG_NAME.tar.gz
```

#### Compiling
Install `libpcre` (and `libssl` if you want https, along with `zlib` for gzip support) with your favorite package manager, naxsi relies on it for regex.

```shell
cd nginx-*
./configure --add-module=../naxsi-x.xx/naxsi_src/ [add/remove your favorite/usual options]
make
make install
```

My personal "building" options being here :

```shell
./configure --conf-path=/etc/nginx/nginx.conf --add-module=../naxsi-x.xx/naxsi_src/ \
--error-log-path=/var/log/nginx/error.log --http-client-body-temp-path=/var/lib/nginx/body \
--http-fastcgi-temp-path=/var/lib/nginx/fastcgi --http-log-path=/var/log/nginx/access.log \
--http-proxy-temp-path=/var/lib/nginx/proxy --lock-path=/var/lock/nginx.lock \
--pid-path=/var/run/nginx.pid --with-http_ssl_module \
--without-mail_pop3_module --without-mail_smtp_module \
--without-mail_imap_module --without-http_uwsgi_module \
--without-http_scgi_module --with-ipv6 --prefix=/usr
```

# IMPORTANT

You need to remember this :
NGINX will decide the order of modules according the order of the module's directive in nginx's `./configure`. So, no matter what (except you really know what you are doing) put naxsi first in your `./configure`.

If you don't do so, you might run into various problems, from random / unpredictable behaviours to non-effective WAF.