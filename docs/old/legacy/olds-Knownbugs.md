Naxsi core :

  * Naxsi only parses GET/POST/PUT requests.
  * Naxsi supports a restricted kind of body content-type parsing : application/x-www-form-urlencoded, multipart/form-data, application/json. Other content-types are not supported.
  * Naxsi does not support parsing of a file buffered to disk by nginx: [client_body_buffer_size](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size)
  * You can't use `|` caracter in your matchzone regular expressions.
  * HTTPv2 is currently not supported: [issue #227](https://github.com/nbs-system/naxsi/issues/227 )