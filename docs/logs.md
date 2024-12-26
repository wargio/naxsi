# **Naxsi Logs**

Naxsi utilizes NGINX's `error_log` feature to produce logs; specifically, it offers two types of error logs that can be generated: one in **URL-encoded format** (which is the default option) and another in **JSON format**.

These logs are `action log` and `extended log`. They include detailed information about the triggered rules, content identified, paths and request IDs for comprehensive security monitoring and analysis.

> ⚠️ Warning
>
> Due to hardcoded limitations of NGINX (see `NGX_MAX_ERROR_STR` defined by `ngx_log.h` in the NGINX source code), these logs may be truncated if too long.
>
> It is strongly recommanded to increase the line limit to `8192` or higher by patching NGINX itself.
>
> Example of patch: `sed -i 's#NGX_MAX_ERROR_STR   2048#NGX_MAX_ERROR_STR   8192#g' src/core/ngx_log.h`

## Action Logs

Action logs contains the following information:

- `ip`: Client IP
- `server`: Nginx server name
- `uri`: Request URI
- `config`: Configuration of naxsi for the applied rule, this can be one of the following values:
	- `learning` [Learning Mode active](directives.md#learningmode), i.e. The [action taken is `LOG`](directives.md#checkrule) and the **request was NOT dropped**. 
	- `learning-drop` [Learning Mode active](directives.md#learningmode) but request was dropped, i.e. [The action taken is `DROP`](directives.md#checkrule).
	- `drop` The request was dropped, i.e. [The action taken is `DROP`](directives.md#checkrule).
	- `block` The request was dropped, i.e. [The action taken is `BLOCK`](directives.md#checkrule).
	- `ignore` The request was supposed to be dropped/blocked, but it was ignored due to matching [`IgnoreIP`](directives.md#ignoreip) or [`IgnoreCIDR`](directives.md#ignorecidr), i.e. [The action taken is `LOG`](directives.md#checkrule).
- `rid`: Request Identifier (matches the `request_id` NGINX value).
- `id<N>`: Matched rule identifier
- `cscore<N>`: Request score name (see [Check Rules](directives.md#checkrule)), for example `$SQL`.
- `score<N>`: Request score value (see [Check Rules](directives.md#checkrule)), for example `8`.
- `zone<N>`: Request [matchzone](matchzones.md), for example `URL`.
- `var_name<N>`: Request variable name where the match has happen, for example `foo_bar[]`.

> ℹ️ Info
>
> The `<N>` is a counter that always starts from `0` for each request and is used for listing all the matched rules and their info (`id`, `cscore`, etc..).
>
> For example, a request can have multiple matching rules and these will be logged by Naxsi as follows:
>
> The first rule (`<N>=0`) is logged as `id0=<id>`, `cscore0=<score name>`, `score0=<score count>`, `zone0=<matchzone>`, `var_name0=<var name>`; the second rule (`<N>=1`) is going to be `id1=<id>`, `cscore1=<score name>`, `score1=<score count>`, `zone1=<matchzone>`, `var_name1=<var name>`, the third (`<N>=2`), etc...
> 
> If the rule score name is the same for multiple rules, the first `cscoreX` defined in the logs will contain the sum of the total score of multiple rules.

## Extended Logs

Extended logs needs to be enabled by adding `set $naxsi_extensive_log 1;` or via the runtime modifier `naxsi_extensive_log` to the NGINX configuration and logs all the matches of a request:

- `ip`: Client IP
- `server`: Nginx server name
- `rid`: Request Identifier (matches the `request_id` NGINX value).
- `uri`: Request URI
- `id`: Matched rule identifier
- `zone`: Request [matchzone](matchzones.md), for example `URL`.
- `var_name`: Request variable name where the match has happen, for example `foo_bar[]`.
- `content`: The matched content, for example `malicious` (can be truncated if too long).


*Action format logs* can be distinguished by *extended logs* the presence of `cscore` & `score` keywords; *Extended logs* are also the only ones having the `content` keyword.

## URL Encoded logs

**This is the default logging format for Naxsi**; These logs are always preceded by `NAXSI_FMT` (action logs) or `NAXSI_EXLOG` (extended logs) and the values of each logged info is `url-encoded`.

Example:

```
2024/12/26 12:27:44 [error] 3829059#0: *4 NAXSI_EXLOG: ip=127.0.0.1&server=localhost&rid=70d8cd8818e7e27a11d14df63c676227&uri=%2Fx%2Cy&id=1015&zone=URL&var_name=&content=%2Fx%2Cy, client: 127.0.0.1, server: localhost, request: "GET /x,y?uuu=b,c HTTP/1.1", host: "localhost"
2024/12/26 12:27:44 [error] 3829059#0: *4 NAXSI_EXLOG: ip=127.0.0.1&server=localhost&rid=70d8cd8818e7e27a11d14df63c676227&uri=%2Fx%2Cy&id=1015&zone=ARGS&var_name=uuu&content=b%2Cc, client: 127.0.0.1, server: localhost, request: "GET /x,y?uuu=b,c HTTP/1.1", host: "localhost"
2024/12/26 12:27:44 [error] 3829059#0: *4 NAXSI_FMT: ip=127.0.0.1&server=localhost&uri=%2Fx%2Cy&config=learning&rid=70d8cd8818e7e27a11d14df63c676227&cscore0=$SQL&score0=8&zone0=URL&id0=1015&var_name0=&zone1=ARGS&id1=1015&var_name1=uuu, client: 127.0.0.1, server: localhost, request: "GET /x,y?uuu=b,c HTTP/1.1", host: "localhost"
```

## JSON logs

This logs format needs to be enabled via `set $naxsi_json_log 1;` or via the runtime modifier `naxsi_json_log`.

*Action logs* can be distinguished by *extended logs* the presence of `cscore` & `score` keywords; *Extended logs* are also the only ones having the `content` keyword.

```
2024/12/26 12:28:37 [error] 3829158#0: *4 {"ip":"127.0.0.1","server":"localhost","rid":"1d27ac3c0b10bbb8783d109213f3f4cd","uri":"/x,y","id":1015,"zone":"URL","var_name":"","content":"/x,y"}, client: 127.0.0.1, server: localhost, request: "GET /x,y?uuu=b,c HTTP/1.1", host: "localhost"
2024/12/26 12:28:37 [error] 3829158#0: *4 {"ip":"127.0.0.1","server":"localhost","rid":"1d27ac3c0b10bbb8783d109213f3f4cd","uri":"/x,y","id":1015,"zone":"ARGS","var_name":"uuu","content":"b,c"}, client: 127.0.0.1, server: localhost, request: "GET /x,y?uuu=b,c HTTP/1.1", host: "localhost"
2024/12/26 12:28:37 [error] 3829158#0: *4 {"ip":"127.0.0.1","server":"localhost","uri":"/x,y","config":"block","rid":"1d27ac3c0b10bbb8783d109213f3f4cd","cscore0":"$SQL","score0":8,"zone0":"URL","id0":1015,"var_name0":"","zone1":"ARGS","id1":1015,"var_name1":"uuu"}, client: 127.0.0.1, server: localhost, request: "GET /x,y?uuu=b,c HTTP/1.1", host: "localhost"
```

Here is an example of how the previous JSON logs appears when formatted by utilities such as `jq`:

```json
// Action Log
{
  "ip": "127.0.0.1",
  "server": "localhost",
  "uri": "/x,y",
  "config": "block",
  "rid": "1d27ac3c0b10bbb8783d109213f3f4cd",
  "cscore0": "$SQL",
  "score0": 8,
  "zone0": "URL",
  "id0": 1015,
  "var_name0": "",
  "zone1": "ARGS",
  "id1": 1015,
  "var_name1": "uuu"
}

// Extended Log
{
  "ip": "127.0.0.1",
  "server": "localhost",
  "rid": "1d27ac3c0b10bbb8783d109213f3f4cd",
  "uri": "/x,y",
  "id": 1015,
  "zone": "URL",
  "var_name": "",
  "content": "/x,y"
}
```