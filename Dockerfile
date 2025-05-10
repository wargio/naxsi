# SPDX-FileCopyrightText: 2025 wargio <deroad@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only

FROM alpine:latest AS build

ENV NGINX_VERSION nginx-1.28.0

COPY naxsi_src /naxsi_src

RUN mkdir -p /tmp/src && \
    apk add --no-cache openssl-dev pcre2-dev zlib-dev wget build-base && \
    cd /tmp/src && \
    wget http://nginx.org/download/${NGINX_VERSION}.tar.gz && \
    tar -zxvf ${NGINX_VERSION}.tar.gz && \
    cd /tmp/src/${NGINX_VERSION} && \
    sed -i 's#NGX_MAX_ERROR_STR   2048#NGX_MAX_ERROR_STR   8192#g' src/core/ngx_log.h && \
    mkdir -p /nginx/logs && \
    ./configure --with-cc-opt='-g -O2 -Wextra -Wall -fstack-protector-strong -Wformat -Werror=format-security -fPIC -Wdate-time -D_FORTIFY_SOURCE=2' \
                --with-ld-opt='-Wl,-z,relro -Wl,-z,now -fPIC' \
                --with-select_module \
                --with-http_ssl_module \
                --with-http_gzip_static_module \
                --without-mail_pop3_module \
                --without-mail_smtp_module \
                --without-mail_imap_module \
                --with-http_realip_module \
                --with-http_v2_module \
                --without-http_uwsgi_module \
                --without-http_scgi_module \
                --with-select_module \
                --add-dynamic-module="/naxsi_src" \
                --http-log-path=/nginx/logs/access.log \
                --error-log-path=/nginx/logs/error.log \
                --prefix=/nginx && \
    make -j && \
    make install
COPY naxsi_rules/ /nginx/naxsi/
COPY distros/nginx/naxsi_learning_mode.conf /nginx/naxsi/
COPY distros/nginx/naxsi_denied_url.conf /nginx/naxsi/
COPY distros/nginx/naxsi_block_mode.conf /nginx/naxsi/

# release
FROM alpine:latest

COPY --from=build /nginx /nginx
RUN \
    apk add --no-cache openssl pcre2 zlib && \
    ln -sf /dev/stdout /nginx/logs/access.log \
    ln -sf /dev/stderr /nginx/logs/error.log

EXPOSE 80

STOPSIGNAL SIGQUIT

CMD ["/nginx/sbin/nginx", "-g", "daemon off;"]
