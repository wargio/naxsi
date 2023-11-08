# Internal rules

Internal rules are rules that can be fired by naxsi, when request is incorrect or extremely unusual - or naxsi is not able to parse the request (ie. unknown content-type).
Please note that those rules do not set an internal score, but usually just set the `block` flag of the request to `1`.

You can whitelist those, but you should never have to do so.
When whitelisting an internal rule, you might be disabling naxsi at least partially, so think twice about it.




## weird_request
 * id: 1
 * action: block
 * impact: pass-thru

A request that cannot be understood by naxsi.
When whitelisting this one, you are telling naxsi to blindly accept the request and not to parse it.

## big_request
 * id: 2
 * action: block
 * impact : pass-thru

A request that is buffered on file system because it's too big.
Naxsi doesn't parse buffered requests. You can always increase [client_body_buffer_size](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size) in nginx's config.

## uncommon_hex_encoding
 * id: 10
 * action: block
 * impact : partial loss of decoding

Hex encoding that is not valid, and that naxsi cannot "url decode".

## uncommon_content_type
 * id: 11
 * action: block
 * impact : pass-thru on BODY

A content-type unknown to naxsi. Meaning naxsi cannot parse the body.
However, if id:11 is whitelisted and >= 0.55rc2, [RAW_BODY](rawbody) rules can be used.

## uncommon_url
 * id: 12
 * action: block
 * impact: partial pass-thru on GET args

An URL that is not standard (ie. `?x=foo&z=bar`). Can lead to uncorrectly parsed arguments when whitelisted.


# uncommon_post_format
 * id: 13
 * action: block
 * impact: pass-thru on BODY

POST body is malformed, ie.
 * bad content-disposition
 * no variable name
 * malformed attached file content-type

## uncommon_post_boundary
 * id: 14
 * action: block
 * impact: pass-thru on BODY

POST body is malformed, ie.
 * bad content-type
 * bad boundary (too short, too long, not rfc compliant)

## invalid_json
 * id: 15
 * action: block
 * impact: pass-thru on BODY (json)

JSON is malformed (ie. missing `} ]`).


## empty_body
 * id: 16
 * action: block
 * impact: pass-thru on BODY

Raised when body is empty and/or content-length is zero.


## libinjection_sql
 * id: 17
 * action: ??

See [libinjection](libinjection-integration.md).

## libinjection_xss
 * id: 18
 * action: ??

See [libinjection](libinjection-integration.md).

## no_rules
 * id: 19
 * action: drop
 * impact: no rules checked

Raised when naxsi isn't configured with any MainRules.

## bad_utf8
 * id: 20
 * action: drop

Raised when [surrogate utf8](https://tools.ietf.org/html/rfc3629) is detected.
