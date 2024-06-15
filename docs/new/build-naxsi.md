# **Installing Naxsi**

In this section you can find how to install and build naxsi on various distributions.

## **Ubuntu/Debian**

Ubuntu & Debian do not provide a package for this, but you can easily compile naxsi using `apt-get source` to fetch the correct version of nginx as follows.

1. **Download the required software**

> [!INFO]
> Some Debian and Ubuntu distributions uses **`libpcre2-dev`** instad of `libpcre3-dev`.

> [!INFO]
> Debian bookworm requires also **`libperl-dev`**

```bash
# Install required software
apt-get install build-essential ca-certificates \
                dpkg-dev zlib1g-dev libgd-dev libgeoip-dev \
                libpcre3-dev libperl-dev libssl-dev libxslt1-dev \
                gzip git nginx tar wget
```

We also need to download **Naxsi**

```bash
NAXSI_VERSION=X.Y
wget "https://github.com/wargio/naxsi/releases/download/$NAXSI_VERSION/naxsi-$NAXSI_VERSION-src-with-deps.tar.gz"
mkdir -p naxsi
tar -C naxsi -xzf naxsi-$NAXSI_VERSION-src-with-deps.tar.gz
```

And fetch the NGINX source via `apt-get source`.

```bash
apt-get source nginx
```

2. **Retrieve the distro compile flags**

To correctly build Naxsi for Debian/Ubuntu, you will need to retrieve the configure arguments (also called `compile flags`) using `nginx -V`, as shown below.

```bash
nginx -V
```

Example of output:

```
nginx version: nginx/1.18.0 (Ubuntu)
built with OpenSSL 1.1.1f  31 Mar 2020
TLS SNI support enabled
configure arguments: --with-cc-opt='-g -O2 -fdebug-prefix-map=/build/nginx-lUTckl/nginx-1.18.0=. -fstack-protector-strong -Wformat -Werror=format-security -fPIC -Wdate-time -D_FORTIFY_SOURCE=2' --with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,now -fPIC' --prefix=/usr/share/nginx --conf-path=/etc/nginx/nginx.conf --http-log-path=/var/log/nginx/access.log --error-log-path=/var/log/nginx/error.log --lock-path=/var/lock/nginx.lock --pid-path=/run/nginx.pid --modules-path=/usr/lib/nginx/modules --http-client-body-temp-path=/var/lib/nginx/body --http-fastcgi-temp-path=/var/lib/nginx/fastcgi --http-proxy-temp-path=/var/lib/nginx/proxy --http-scgi-temp-path=/var/lib/nginx/scgi --http-uwsgi-temp-path=/var/lib/nginx/uwsgi --with-debug --with-compat --with-pcre-jit --with-http_ssl_module --with-http_stub_status_module --with-http_realip_module --with-http_auth_request_module --with-http_v2_module --with-http_dav_module --with-http_slice_module --with-threads --with-http_gzip_static_module --without-http_browser_module --without-http_geo_module --without-http_limit_req_module --without-http_limit_conn_module --without-http_memcached_module --without-http_referer_module --without-http_split_clients_module --without-http_userid_module --add-dynamic-module=/build/nginx-lUTckl/nginx-1.18.0/debian/modules/http-echo
```

To simplify this process, you can use the following command, which takes the output of `nginx -V` and modifies it; this can be used as a quick way to get "ready-to-use" configure arguments for building NGINX.

```bash
nginx -V 2>&1 | grep "configure arguments:" | cut -d ":" -f2- | sed -e "s#/build/nginx-[A-Za-z0-9]*/#./#g" | sed 's/--add-dynamic-module=[A-Za-z0-9\/\._-]*//g'
```

Example of output:

```
--with-cc-opt='-g -O2 -fdebug-prefix-map=./nginx-1.18.0=. -fstack-protector-strong -Wformat -Werror=format-security -fPIC -Wdate-time -D_FORTIFY_SOURCE=2' --with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,now -fPIC' --prefix=/usr/share/nginx --conf-path=/etc/nginx/nginx.conf --http-log-path=/var/log/nginx/access.log --error-log-path=/var/log/nginx/error.log --lock-path=/var/lock/nginx.lock --pid-path=/run/nginx.pid --modules-path=/usr/lib/nginx/modules --http-client-body-temp-path=/var/lib/nginx/body --http-fastcgi-temp-path=/var/lib/nginx/fastcgi --http-proxy-temp-path=/var/lib/nginx/proxy --http-scgi-temp-path=/var/lib/nginx/scgi --http-uwsgi-temp-path=/var/lib/nginx/uwsgi --with-debug --with-compat --with-pcre-jit --with-http_ssl_module --with-http_stub_status_module --with-http_realip_module --with-http_auth_request_module --with-http_v2_module --with-http_dav_module --with-http_slice_module --with-threads --with-http_gzip_static_module --without-http_browser_module --without-http_geo_module --without-http_limit_req_module --without-http_limit_conn_module --without-http_memcached_module --without-http_referer_module --without-http_split_clients_module --without-http_userid_module 
```

3. **Build NGINX with Naxsi as module**

Now we will build Naxsi using NGINX sources:

```bash
# Build NGINX with Naxsi
cd nginx-*
NGINX_BUILD_FLAGS=$(nginx -V 2>&1 | grep "configure arguments:" | cut -d ":" -f2- | sed -e "s#/build/nginx-[A-Za-z0-9]*/#./#g" | sed 's/--add-dynamic-module=[A-Za-z0-9\/\._-]*//g')
./configure $NGINX_BUILD_FLAGS --add-dynamic-module=../naxsi/naxsi_src/
make modules
```

You will find the built module at the following path:

```
nginx-<version>/objs/ngx_http_naxsi_module.so
```

The other files you will need, are **the rules**, which can be found at the following path:

```
naxsi/naxsi_rules
```

> [!WARNING]
> **Be aware that you may encounter the following error related to `libinjection`, which can be safely ignored.**

```
[truncated output ...]
configuring additional dynamic modules
adding module in ../naxsi/naxsi_src
Package libinjection was not found in the pkg-config search path.
Perhaps you should add the directory containing `libinjection.pc'
to the PKG_CONFIG_PATH environment variable
No package 'libinjection' found
Package libinjection was not found in the pkg-config search path.
Perhaps you should add the directory containing `libinjection.pc'
to the PKG_CONFIG_PATH environment variable
No package 'libinjection' found
Using submodule libinjection
 + naxsi was configured  
```

# **Compiling Naxsi from Sources**

> [!INFO]
> You will need to have a working C dev environment installed on your system, for tools like `gcc` or `clang` and `make`, in order to compile Naxsi.

> [!WARNING]
> You will need to have `libpcre` or `libpcre2` or `libpcre3` installed to correctly build Naxsi.

To compile Naxsi from source code, follow these steps:

1. **Get Naxsi sources**

```bash
NAXSI_VERSION=X.Y
wget "https://github.com/wargio/naxsi/releases/download/$NAXSI_VERSION/naxsi-$NAXSI_VERSION-src-with-deps.tar.gz"
mkdir -p naxsi
tar -C naxsi -xzf naxsi-$NAXSI_VERSION-src-with-deps.tar.gz
```

2. **Get NGINX sources**

```bash
NGINX_VERSION=X.Y.Z
wget https://nginx.org/download/nginx-$NGINX_VERSION.tar.gz
mkdir -p nginx
tar -C nginx -xzf nginx-$NGINX_VERSION.tar.gz
```

3. **Build NGINX and Naxsi**

```
cd nginx
./configure --add-dynamic-module=../naxsi/naxsi_src/
make modules
```

4. **Install Nginx and Naxsi**

You can automatically install the files using `make install` or alternatively you can manually install the built module using:

You will find the built module at the following path:

```
nginx/objs/ngx_http_naxsi_module.so
```

The other files you will need, are **the rules**, which can be found at the following path:

```
naxsi/naxsi_rules
```

# Next Page

Next: [Basic Configuration](basic-configuration.md).