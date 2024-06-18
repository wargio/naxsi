# DeniedUrl

DeniedUrl is used to indicate a location where blocked requests will be redirected (internally).

In version before 0.49, by default, naxsi forwards blocked request there while in learning mode. Upon "real" request termination, using nginx's post_action mechanism.

This was due to usage of nx_intercept, which could intercept learning traffic in live.

As the request might be modified during redirect (url & arguments), extra http headers orig_url (original url), orig_args (original GET args) and naxsi_sig (NAXSI_FMT) are added.

If $naxsi_flag_post_action is set to "1", naxsi will perform post_action (while in learning) even in versions '''> 0.49'''.

The headers that are forwarded to the location denied page are :
  * Naxsi orig url : x-orig_url header
  * Naxsi orig args : x-orig_args header
  * Naxsi orig sig : x-naxsi_sig header
