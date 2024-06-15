**DeniedUrl** is used to indicate a location where blocked requests will be redirected (internally).

In version before 0.49, by default, naxsi forwards blocked request there while in learning mode. Upon "real" request termination, using nginx's [post_action]( http://wiki.nginx.org/HttpCoreModule#post_action ) mechanism. 

This was due to usage of `nx_intercept`, which could intercept learning traffic in live.

As the request might be modified during redirect (url & arguments), extra http headers `orig_url` (original url), `orig_args` (original GET args) and `naxsi_sig` (NAXSI_FMT) are added.

If $naxsi_flag_post_action is set to "1", naxsi will perform post_action (while in learning) even in versions '''> 0.49'''.


The headers that are forwarded to the location denied page are :

```
./naxsi_runtime.c:  #define NAXSI_HEADER_ORIG_URL "x-orig_url"
./naxsi_runtime.c:  #define NAXSI_HEADER_ORIG_ARGS "x-orig_args"
./naxsi_runtime.c:  #define NAXSI_HEADER_NAXSI_SIG "x-naxsi_sig"
```

