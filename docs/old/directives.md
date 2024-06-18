# Directives

## DeniedUrl 
 * alias: denied_url
 * context: location

`DeniedUrl` is a directive that indicates where naxsi will redirect (nginx's internal redirect) blocked requests.

As the request might be modified during redirect (url & arguments), extra http headers orig_url (original url), 
orig_args (original GET args) and naxsi_sig (NAXSI_FMT) are added.

The headers that are forwarded to the location denied page are :

NAXSI_HEADER_ORIG_URL "x-orig_url"
NAXSI_HEADER_ORIG_ARGS "x-orig_args"
NAXSI_HEADER_NAXSI_SIG "x-naxsi_sig"

example:
```
location / {
...
DeniedUrl "/RequestDenied";
}

location /RequestDenied {
return 418; #I'm a teapot
}
```

## LearningMode
 * alias: learning_mode
 * context: location

`LearningMode` if instructs naxsi to enable learning mode (don't honor `BLOCK` directive) in the location.

For example:

```nginx
location /a {
# request triggering BLOCK score won't be blocked here, but simply logued.
LearningMode;
}
```

Keep in mind that internal rules (those with an `id` inferior to 1000) will drop the request even in learning mode, because it means that something fishy is going on, since naxsi can't correctly process the request.
You can of course apply [whitelist](whitelists-bnf.md) if those are false-positives.

## SecRulesEnabled
 * alias: rules_enabled
 * context: location

`SecRulesEnabled` is a mandatory keyword to enable naxsi in a location.

## SecRulesDisabled
 * alias: rules_disabled
 * context: location

`SecRulesDisabled` can be used to explicitely disable naxsi in a location.

## CheckRule
 * alias: check_rule
 * context: location

See [CheckRule](checkrules-bnf.md)

## BasicRule
 * alias: basic_rule
 * context: location

A directive used to declare a [rule](rules-bnf.md) or a [whitelist](whitelist-bnf.md).

## MainRule
 * alias: main_rule
 * context: http

A directive used to declare a [rule](rule-bnf.md) or a [whitelist](whitelist-bnf.md).

## LibInjectionXss
 * alias: libinjection_xss
 * context: location

A directive to enable [libinjection's xss detection](libinjection-integration.md) on *all* part of the http request.

## LibInjectionSql
 * alias: libinjection_sql
 * context: location

A directive to enable [libinjection's sqli detection](libinjection-integration.md) on *all* part of the http request.

## naxsi_extensive_log
 * context: server
 
A flag that can be set at [runtime](runtime-modifiers.md) to enable [naxsi extensive logs](naxsilogs.md#naxsi_exlog).

```
server {
...

 if ($remote_addr = "1.2.3.4") {
  set $naxsi_extensive_log 1;
 }
 
location / {
 ...
 }
}
```

## naxsi_json_log
 * context: server

Enable JSON in logs. Examples:

```
# normal log in JSON format
2022/12/22 20:36:35 [error] 1189262#0: *1 {"ip":"127.0.0.1","server":"localhost","uri":"/a","config":"block","rid":"a0333f697ff8f12b6a200a24117ff320","cscore0":"$SQL","score0":"8","cscore1":"$XSS","score1":"8","zone0":"ARGS","id0":"1001","var_name0":"b"}, client: 127.0.0.1, server: localhost, request: "GET /a?b="\dasdasdasdadsa HTTP/1.1", host: "localhost"
# extended log in json format
2022/12/22 20:36:35 [error] 1189262#0: *1 {"ip":"127.0.0.1","server":"localhost","uri":"/a","config":"block","rid":"a0333f697ff8f12b6a200a24117ff320","cscore0":"$SQL","score0":"8","cscore1":"$XSS","score1":"8","zone0":"ARGS","id0":"1001","var_name0":"b"}, client: 127.0.0.1, server: localhost, request: "GET /a?b="\dasdasdasdadsa HTTP/1.1", host: "localhost"
```

TODO DOCUMENTATION

## naxsi_flag_enable
 * context: server
 
A flag that can be set at [runtime](runtime-modifiers.md) to enable or disable naxsi.

```
server {
 set $naxsi_flag_enable 1;
 location / {
 ...
 }
}
```

## naxsi_flag_learning
 * context: server

A flag that can be set at [runtime](runtime-modifiers.md) to enable or disable learning.

```
server {
 set $naxsi_flag_learning 1;
 location / {
 ...
 }
}
```

## naxsi_flag_libinjection_sql
 * context: server

A flag that can be set at [runtime](runtime-modifiers.md) to enable or disable [libinjection's sql detection](libinjection-integration.md)

```
server {
 set $naxsi_flag_libinjection_sql 1;
 location / {
 ...
 }
}
```

## naxsi_flag_libinjection_xss

A flag that can be set at [runtime](runtime-modifiers.md) to enable or disable [libinjection's xss detection](libinjection-integration.md)

```
server {
 set $naxsi_flag_libinjection_xss 1;
 location / {
 ...
 }
}
```

