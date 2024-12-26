# **Naxsi Matchzones**

**Matchzones**, denoted by the prefix `mz:`, are crucial components of **rules** and **whitelists**. They act like filters to define specific locations where a pattern should be searched or allowed. 

Here's how they function differently based on the context:

- **Rules:** In this case, matchzones work with an **OR** logic (like `BODY` OR `HEADERS`). This means that as long as one of the specified conditions is met, the rule triggers.

- **Whitelists:** Here, matchzones operate under an **AND** logic (like `url` must be `/foo` AND must occur in `ARGS`). It requires that *both conditions* within a whitelist be satisfied before the pattern is allowed.

> 📣 Important
>
> Naxsi decodes any `url-encoded` or `hexadecimal` sequence, this means the string or regex to search for must be of the decoded content (**this applies also to URLs**).
>
> Example: `1%20UnioN%20SeLEct%201` becomes `1 UnioN SeLEct 1` before applying rules. 

## Any Matchzone

This special matchzone designated by `ANY` allows to define **rules and whitelists which matches in any area of a request**.

For instance, the rule `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:ANY";` is equivalent of writing the following rules but in one line.

```
MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:ARGS|HEADERS|BODY|URL";
MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:RAW_BODY";
MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:FILE_EXT";
```

> 📣 Important
>
> This can be used also for whitelists, but it is possible to disable a rule by just not declaring any matchzone (see the [Whitelist matchzones notes](whitelist.md#matchzone)).

## Filter by HTTP Headers

HTTP headers let the client and the server pass additional information with a message in a request or response; Naxsi allows to filter rules and whitelists by headers as follows:

### Filter by Any HTTP header value

This Matchzone designated by `HEADERS` is specifically tailored to identify **only the content found within HTTP headers**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:HEADERS";` detects the occurrence of the string `malicious` exclusively within the HTTP headers values transmitted in a request.
- A whitelist entry like `BasicRule wl:12345 "mz:HEADERS";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP header values.

### Filter by Any HTTP header name

This Matchzone designated by `HEADERS|NAME` is specifically tailored to identify **only the name of the header found within HTTP request**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8" "str:x-forward-to" "mz:HEADERS|NAME";` detects the occurrence of the string `X-Forward-To` exclusively within the HTTP headers names transmitted in a request.
- A whitelist entry like `BasicRule wl:12345 "mz:HEADERS|NAME";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP header name.

### Filter by HTTP header name

This Matchzone designated by `$HEADERS_VAR:foo` and `$HEADERS_VAR_X:^foo$` is specifically tailored to identify **only the content found within an HTTP header named `foo`**.

- `$HEADERS_VAR:<string>` can be used to filter by header name (**case-insensitive**) via a string.
- `$HEADERS_VAR_X:<regex>` can be used to filter by header name (**case-insensitive**) via a regex.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:curl" "mz:$HEADERS_VAR:user-agent|$HEADERS_VAR:cookie";` detects the occurrence of the string `curl` exclusively within the value of the HTTP headers `User-Agent` and `Cookie` (**case-insensitive**).
- A whitelist entry like `BasicRule wl:12345 "mz:$HEADERS_VAR_X:^cookie$";` negates the match of any rule with id `12345` if the match occurs within the value of the HTTP header `Cookie` via regex.

> 📣 Important
>
> This can be mixed with `|NAME` to perform the filtering at argument name instead of value.
> Example: `mz:$HEADERS_VAR_X:^foo\d+$|NAME` matches only the HTTP header named `foo<number>`.

## Filter by GET query

HTTP GET requests can carry information, referred as queries, in the form of key=value pairs; Naxsi allows to filter rules and whitelists by these arguments as follows:

### Filter by Any GET query value

This Matchzone designated by `ARGS` is specifically tailored to identify **only the value found within HTTP GET query**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:ARGS";` detects the occurrence of the string `malicious` exclusively within the HTTP GET queries values transmitted in a request.
- A whitelist entry like `BasicRule wl:12345 "mz:ARGS";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP GET queries values.

### Filter by Any GET query name

This Matchzone designated by `ARGS|NAME` is specifically tailored to identify **only the name of the GET query found within HTTP request**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8" "str:delete_action" "mz:ARGS|NAME";` detects the occurrence of the string `delete_action` exclusively within the HTTP GET queries names transmitted in a request.
- A whitelist entry like `BasicRule wl:12345 "mz:ARGS|NAME";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP GET queries names.

### Filter by GET query value or name

This Matchzone designated by `$ARGS_VAR:foo` and `$ARGS_VAR_X:^foo$` is specifically tailored to identify **only the content found within an HTTP GET query named `foo`**.

- `$ARGS_VAR:<string>` can be used to filter by argument name (**case-insensitive**) via a string.
- `$ARGS_VAR_X:<regex>` can be used to filter by argument name (**case-insensitive**) via a regex.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:evil" "mz:$ARGS_VAR:foo|$ARGS_VAR:bar";` detects the occurrence of the string `evil` exclusively within the value of the GET queries `User-Agent` (**case-insensitive**).
- A whitelist entry like `BasicRule wl:12345 "mz:$ARGS_VAR_X:^cookie$";` negates the match of any rule with id `12345` if the match occurs within the value of the HTTP header `Cookie` via regex.

> 📣 Important
>
> This can be mixed with `|NAME` to perform the filtering at argument name instead of value.
> Example: `mz:$ARGS_VAR_X:^foo\d+$|NAME` matches only the GET query named `foo<number>`.

## Filter by POST Requests

HTTP POST requests carries information in the HTTP body; the request data can have multiple formats:

- `application/x-www-form-urlencoded` contains key=value pairs.
- `multipart/form-data` contains boundaries with the raw data.

Naxsi allows to filter these in rules and whitelists as follows:

### Filter by Any `application/x-www-form-urlencoded` Value

This Matchzone designated by `BODY` is specifically tailored to identify **only the value found within HTTP POST body**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:BODY";` detects the occurrence of the string `malicious` exclusively within the HTTP POST body values (key=value format) transmitted in a request.
- A whitelist entry like `BasicRule wl:12345 "mz:BODY";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP POST body values.

### Filter by Any `application/x-www-form-urlencoded` Key

This Matchzone designated by `BODY|NAME` is specifically tailored to identify **only the name of the header found within HTTP request**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8" "str:delete_action" "mz:BODY|NAME";` detects the occurrence of the string `delete_action` exclusively within the HTTP POST arguments names transmitted in a request.
- A whitelist entry like `BasicRule wl:12345 "mz:BODY|NAME";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP POST arguments names.

### Filter by Any `application/x-www-form-urlencoded` Key and Value

This Matchzone designated by `$BODY_VAR:foo` and `$BODY_VAR_X:^foo$` is specifically tailored to identify **only the content found within an HTTP POST body named `foo`**.

- `$BODY_VAR:<string>` can be used to filter by POST form name (**case-insensitive**) via a string.
- `$BODY_VAR_X:<regex>` can be used to filter by POST form name (**case-insensitive**) via a regex.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:malicious" "mz:$BODY_VAR:foo|$BODY_VAR:bar";` detects the occurrence of the string `malicious` exclusively within the value of the POST form keys `foo` and `body` (**case-insensitive**).
- A whitelist entry like `BasicRule wl:12345 "mz:$BODY_VAR_X:^foo$";` negates the match of any rule with id `12345` if the match occurs within the value of the POST form named `foo` via regex.

> 📣 Important
>
> This can be mixed with `|NAME` to perform the filtering at argument name instead of value.
> Example: `mz:$BODY_VAR_X:^foo\d+$|NAME` matches only the POST argument named `foo<number>`.

### Filter by Any `multipart/form-data` filename

This Matchzone designated by `FILE_EXT` is specifically tailored to match **only the filename found within HTTP multipart POST request**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:.php" "mz:FILE_EXT";` detects the occurrence of the string `.php` exclusively within the filename of the HTTP multipart POST request.
- A whitelist entry like `BasicRule wl:12345 "mz:FILE_EXT";` negates the match of any rule with id `12345` if the match itself occurs in the filename of the HTTP multipart POST request.

## Filter by HTTP Raw Body

This Matchzone designated by `RAW_BODY` is specifically tailored to match **any byte sequence in an unparsed HTTP body**.

For instance:

- A rule such as `BasicRule id:12345 "s:$EXECUTABLE:8" "rx:MZ\x90" "mz:RAW_BODY";` detects the occurrence of a byte sequence (Windows PE magic) within the HTTP body of the request.
- A whitelist entry like `BasicRule wl:12345 "mz:RAW_BODY";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP body of the request.

> 📣 Important
>
> This matchzone is internally converted as `BODY` zone, thus the `BasicRule wl:12345 "mz:BODY";` and `BasicRule wl:12345 "mz:RAW_BODY";` are equivalent.

## Filter by HTTP URL

Naxsi supports filtering by HTTP URL as follows:

### Filter by HTTP URL (global)

This Matchzone designated by `URL` is specifically tailored to identify **only the value found within HTTP URL**.

For instance:

- A rule such as `MainRule id:12345 "s:$FOO:8,$BAR:4" "str:/admin" "mz:URL";` detects the occurrence of the string `/admin` exclusively within the HTTP URL transmitted in a request.
- A whitelist entry like `BasicRule wl:12345 "mz:URL";` negates the match of any rule with id `12345` if the match itself occurs in the HTTP URL.

> 📣 Important
>
> This matchzone is globally applied, it is possible to restrict the matchzone to a specific URL or substring in the URL via `$URL` or `$URL_X` (see below).

### Filter by HTTP URL (restricted)

This Matchzone designated by `$URL:/foo` and `$URL_X:^/foo$` is specifically tailored to identify **only the URL containing the string `/foo`**.

- `$URL:<string>` can be used to filter by string (**case-insensitive**).
- `$URL_X:<regex>` can be used to filter by regex (**case-insensitive**).

These can be mixed with all the previous matchzones as follows:

In rules context, `$URL` or `$URL_X` *must* be satisfied if present. Any other condition is treated as *OR* (opposite to whitelists).

- The rule `BasicRule str:Y id:X "mz:ARGS|BODY";` is interpreted as _pattern 'Y' will be matched against *any* GET and POST arguements_
- The rule `BasicRule str:Y id:X "mz:ARGS|BODY|$URL:/foo";` is interpreted as _pattern 'Y' will be matched against *any* GET and POST arguements as long as URL is `/foo`_

In whitelist context, *all* conditions must be satisfied, so a whitelist like `BasicRule wl:X "mz:$ARGS_VAR:foo|$URL:/bar";` is interpreted as _id X is whitelisted in GET variable `foo` on URL `/bar`_

> ⚠️ Warning
>
> **You CANNOT mix `$URL_X:<regex>` and `$ARGS_VAR:<string>`, `$BODY_VAR:<string>` and `$HEADERS_VAR:<string>` in a rule or whitelist.**
>
> It is allowed instead to mix `$URL_X:<regex>` with `$ARGS_VAR_X:<regex>`, `$BODY_VAR_X:<regex>` and `$HEADERS_VAR_X:<regex>` and to mix `$URL:<string>` with `$ARGS_VAR:<string>`, `$BODY_VAR:<string>` and `$HEADERS_VAR:<string>`.

> ℹ️ Info
>
> It is allowed to mix `FILE_EXT` and `RAW_BODY` with `$URL_X:<regex>` and `$URL:<string>`.

# Go Back

[Table of Contents](index.md).
