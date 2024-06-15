# **Internal Rules**

Naxsi has some internal rules that are hardcoded within the WAF; these rules are defined by **ids** lower than **1000**.

> 📣 Important
>
> The internal blocking rules can be whitelisted.

> ⚠️ Warning
>
> No rules shall be defined with **ids** lower than 1000.

## Internal Rule 1 - Weird Request

> ❌ **Deprecated**
>
> Number: **1**
> Name: **Weird Request**
> Action: **BLOCK**

The internal rule `1` refers to any request that contains weird request which failed to be parsed by Naxsi.

## Internal Rule 2 - Big Request

> ℹ️ Info
>
> Number: **2**
> Name: **Big Request**
> Action: **BLOCK**

The internal rule `2` refers to any request that is too big to be parsed; this only happens when NGINX has to create a temporary file on the filesystem or when the content-size mismatch with the actual body size.

## Internal Rule 10 - Hex Encoded Null-Bytes

> ℹ️ Info
>
> Number: **10**
> Name: **Null-Byte Hex Encoding**
> Action: **BLOCK**

The internal rule `10` refers to any request that contains one or many hex encoded null-bytes (i.e. `0x00` or `\x00`).

## Internal Rule 11 - Uncommon Content Type

> ℹ️ Info
>
> Number: **11**
> Name: **Uncommon Content Type**
> Action: **BLOCK**

The internal rule `11` refers to any request that contains uncommon content type; this happens when `Content-Type` header is missing or during a POST request, the `Content-Type` is not one of the followings:

- `"application/x-www-form-urlencoded"`
- `"multipart/form-data"`
- `"application/json"`
- `"application/vnd.api+json"`
- `"application/csp-report"`

## Internal Rule 12 - Invalid formatted URL

> ℹ️ Info
>
> Number: **12**
> Name: **Invalid formatted URL**
> Action: **BLOCK**

The internal rule `12` refers to any request that contains a badly formatted URL; this happens when the HTTP request has an invalid URL (this may be caught before-hand by NGINX which may return 400).

## Internal Rule 13 - Malformed POST Format

> ℹ️ Info
>
> Number: **13**
> Name: **Malformed POST Format**
> Action: **BLOCK**

The internal rule `13` refers to any request that contains a malformed POST, for example missing `content-disposition`, malformed boundary line, missing name, missing `Content-Type`, etc...

## Internal Rule 14 - Malformed POST Boundary

> ℹ️ Info
>
> Number: **14**
> Name: **Malformed POST Boundary**
> Action: **BLOCK**

The internal rule `14` refers to any request that contains a malformed POST boundary.

## Internal Rule 15 - Malformed JSON

> ℹ️ Info
>
> Number: **15**
> Name: **Malformed JSON**
> Action: **BLOCK**

The internal rule `15` refers to any request that contains malformed JSON.

## Internal Rule 16 - Empty POST Body

> ℹ️ Info
>
> Number: **16**
> Name: **Empty POST Body**
> Action: **BLOCK**

The internal rule `16` refers to any request that contains empty POST body.

## Internal Rule 17 - libinjection SQLi

> ℹ️ Info
>
> Number: **17**
> Name: **libinjection SQLi**
> Score: **$LIBINJECTION_SQL**

> ⚠️ Warning
>
> This rule does not block a request, but increases the score `$LIBINJECTION_SQL` by **1**.

The internal rule `17` refers to any request that contains sql injections detected by libinjection.

See also [Directive `LibInjectionSql`](directives.md#libinjectionsql) for more details.

## Internal Rule 18 - libinjection XSS

> ℹ️ Info
>
> Number: **18**
> Name: **libinjection Xss**
> Score: **$LIBINJECTION_XSS**

> ⚠️ Warning
>
> This rule does not block a request, but increases the score `$LIBINJECTION_XSS` by **1**.

The internal rule `18` refers to any request that contains XSS injections detected by libinjection.

See also [Directive `LibInjectionXss`](directives.md#libinjectionxss) for more details.

## Internal Rule 19 - No Rules Loaded

> ℹ️ Info
>
> Number: **19**
> Name: **No Rules**
> Action: **DROP**

The internal rule `19` is triggered only when the WAF is enabled but no global and no location-specific rules has been loaded at the current location.

## Internal Rule 20 - Malformed UTF-8

> ℹ️ Info
>
> Number: **20**
> Name: **Malformed UTF-8**
> Action: **DROP**

The internal rule `20` refers to any request that contains malformed UTF-8.

## Internal Rule 21 - Illegal Host in Header

> ℹ️ Info
>
> Number: **21**
> Name: **Illegal Host in Header**
> Action: **DROP**

The internal rule `21` refers to any request that contains a host header with an illegal ip:

- `0.0.0.0/8`
- `255.255.255.255/32`
- `0000:0000:0000:0000:0000:0000:0000:0000/128`
- `ff00:0000:0000:0000:0000:0000:0000:0000/8`

# Go Back

[Rules](rules.md).
