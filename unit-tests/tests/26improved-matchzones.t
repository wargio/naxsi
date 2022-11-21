#vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

plan tests => repeat_each(1) * (blocks() + 10);
no_root_location();
no_long_string();
$ENV{TEST_NGINX_SERVROOT} = server_root();
run_tests();

__DATA__
=== TEST 1.0: blacklist on static var name (good)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratz" "mz:$ARGS_VAR:foo1|$URL:/ff" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR:foo" "s:$XSS:8";
MainRule id:4241 "str:ratz" "mz:$ARGS_VAR:foo1" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo=ratataXXX
--- error_code: 412
=== TEST 1.1: blacklist on static var name (bad)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR:foo1/$URL:/zz" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR:foo" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR:foo/$URL:/zz" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foox=ratataXXX
--- error_code: 200
=== TEST 2.0: blacklist on rx var name (good)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^foa[0-9]+$|$URL_X:/f1" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^foo[0-9]+$" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^foa[0-9]+$|$URL_X:/ff" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^foa[0-9]+$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo888=ratataXXX
--- error_code: 412
=== TEST 2.1: blacklist on rx var name (bad)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^foo[0-9]+$/$URL_X:/z" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^foo[0-9]+$" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^fo1[0-9]+$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foob=ratataXXX
--- error_code: 200
=== TEST 3.0: blacklist on rx var name (bad zone)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$BODY_VAR_X:^foo[0-9]+$|$URL_X:/fz" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$BODY_VAR_X:^foo[0-9]+$" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$BODY_VAR_X:^fo1[0-9]+$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo88=ratataXXX
--- error_code: 200

=== TEST 3.1: blacklist on static var name (bad zone)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$BODY_VAR:foo|$URL:/f" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$BODY_VAR:foo" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo=ratataXXX
--- error_code: 200

=== TEST 4.0: blacklist on multi static var name (one good, many bad)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL:/zz|$ARGS_VAR:aaa|$ARGS_VAR:foo|$ARGS_VAR:nope" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR:aaa|$ARGS_VAR:foo|$ARGS_VAR:nope" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo=ratataXXX
--- error_code: 412

=== TEST 4.1: blacklist on multi rx var name (one good, many bad)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL_X:^/z$|$ARGS_VAR_X:^aaa$|$ARGS_VAR_X:^foo$|$ARGS_VAR_X:^nope$" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^aaa$|$ARGS_VAR_X:^foo$|$ARGS_VAR_X:^nope$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo=ratataXXX
--- error_code: 412


=== TEST 5.0: blacklist on multi rx var name (many bad)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^aaa$|$ARGS_VAR_X:^foo$|$ARGS_VAR_X:^nope$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo1=ratataXXX
--- error_code: 200


=== TEST 5.1: blacklist on multi rx var name (many bad, one good zone)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR_X:^aaa$|$ARGS_VAR_X:^foo$|$ARGS_VAR_X:^nope$|ARGS" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo1=ratataXXX
--- error_code: 412


=== TEST 6.0: blacklist on multi static var name (many bad)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR:^aaa$|$ARGS_VAR:^foo$|$ARGS_VAR:^nope$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo1=ratataXXX
--- error_code: 200


=== TEST 6.1: blacklist on multi static var name (many bad, one good zone)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$ARGS_VAR:^aaa$|$ARGS_VAR:^foo$|$ARGS_VAR:^nope$|ARGS" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foo1=ratataXXX
--- error_code: 412

=== TEST 7.0: static blacklist on $URL:/ | $ARGS_VAR  (both good)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL:/fooa|$ARGS_VAR:aaa" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$URL:/foo|$ARGS_VAR:aaa" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$URL:/fooz|$ARGS_VAR:aaa" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foo?aaa=ratataXXX
--- error_code: 412


=== TEST 7.1: static blacklist on $URL:/ | $ARGS_VAR  (bad url)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL:/foo1|$ARGS_VAR:aaa" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$URL:/foo|$ARGS_VAR:aaa" "s:$XSS:8";
MainRule id:4241 "str:ratata" "mz:$URL:/foo2|$ARGS_VAR:aaa" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foox?aaa=ratataXXX
--- error_code: 404



=== TEST 7.2: static blacklist on $URL:/ | $ARGS_VAR  (bad ARGS_VAR)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL:/foo|$ARGS_VAR:aaa" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foo?axaa=ratataXXX
--- error_code: 404

=== TEST 7.3: static blacklist on $URL:/ | $ARGS_VAR  (one bad ARGS_VAR and one good)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL:/foo|$ARGS_VAR:aaa|$ARGS_VAR:tutu" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foo?tutu=ratataXXX
--- error_code: 412


=== TEST 7.0: rx blacklist on $URL_X:/ | $ARGS_VAR_X  (both good)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL_X:^/foo$|$ARGS_VAR_X:^aaa[0-9]+$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foo?aaa4242=ratataXXX
--- error_code: 412


=== TEST 7.1: rx blacklist on $URL_X:/ | $ARGS_VAR_X  (bad url)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL_X:^/foo$|$ARGS_VAR_X:^aaa$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foox?aaa=ratataXXX
--- error_code: 404



=== TEST 7.2: rx blacklist on $URL_X:/ | $ARGS_VAR_X  (bad ARGS_VAR)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL_X:^/foo$|$ARGS_VAR_X:^aaa$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foo?axaa=ratataXXX
--- error_code: 404

=== TEST 7.3: static blacklist on $URL:/ | $ARGS_VAR  (one bad ARGS_VAR and one good)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:4241 "str:ratata" "mz:$URL_X:^/foo$|$ARGS_VAR_X:^aaa$|$ARGS_VAR_X:^tutu$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /foo?tutu=ratataXXX
--- error_code: 412


=== TEST 8.0: gni ?
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:foobar" "mz:$ARGS_VAR_X:^foo.*" "msg:lol" "s:DROP" id:42424242;
#MainRule id:4241 "str:ratata" "mz:$URL_X:^/foo$|$ARGS_VAR_X:^aaa$|$ARGS_VAR_X:^tutu$" "s:$XSS:8";
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?fooxxxad=foobar
--- error_code: 412


=== TEST 9.0: ANY in MainRule (ARGS)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?fooxxxad=foobar
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=ARGS&id0=42424242&var_name0=fooxxxad, client: 127\.0\.0\.1,@


=== TEST 9.1: ANY in MainRule (ARGS|NAME)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}
--- request
GET /?foobar=fooxxxad
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=ARGS|NAME&id0=42424242&var_name0=foobar, client: 127\.0\.0\.1,@


=== TEST 9.2: ANY in MainRule (BODY)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
    SecRulesEnabled;
    DeniedUrl "/RequestDenied";
    CheckRule "$SQL >= 8" BLOCK;
    CheckRule "$RFI >= 8" BLOCK;
    CheckRule "$TRAVERSAL >= 4" BLOCK;
    CheckRule "$XSS >= 8" BLOCK;
    CheckRule "$TEST >= 8" ALLOW;
    BasicRule wl:11 "mz:BODY|RAW_BODY";

    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;
}
location /RequestDenied {
    return 412;
}
--- raw_request eval
"POST / HTTP/1.1\r
Host: 127.0.0.1\r
Connection: Close\r
User-Agent: Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10\r
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r
Accept-Language: en-us,en;q=0.5\r
Accept-Encoding: gzip, deflate\r
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r
Referer: http://127.0.0.1/\r
Content-Type: application/x-www-form-urlencoded\r
Content-Length: 11\r
\r
aaaa=foobar\r
"
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=BODY&id0=42424242&var_name0=aaaa, client: 127\.0\.0\.1,@


=== TEST 9.2: ANY in MainRule (BODY|NAME)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
    SecRulesEnabled;
    DeniedUrl "/RequestDenied";
    CheckRule "$SQL >= 8" BLOCK;
    CheckRule "$RFI >= 8" BLOCK;
    CheckRule "$TRAVERSAL >= 4" BLOCK;
    CheckRule "$XSS >= 8" BLOCK;
    CheckRule "$TEST >= 8" ALLOW;
    BasicRule wl:11 "mz:BODY|RAW_BODY";

    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;
}
location /RequestDenied {
    return 412;
}
--- raw_request eval
"POST / HTTP/1.1\r
Host: 127.0.0.1\r
Connection: Close\r
User-Agent: Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10\r
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r
Accept-Language: en-us,en;q=0.5\r
Accept-Encoding: gzip, deflate\r
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r
Referer: http://127.0.0.1/\r
Content-Type: application/x-www-form-urlencoded\r
Content-Length: 11\r
\r
foobar=aaaa\r
"
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=BODY|NAME&id0=42424242&var_name0=foobar, client: 127\.0\.0\.1,@


=== TEST 9.3: ANY in MainRule (HEADERS)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}

--- more_headers
SomeWeirdHeader: foobar32
--- request
GET /
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=HEADERS&id0=42424242&var_name0=someweirdheader, client: 127\.0\.0\.1,@


=== TEST 9.4: ANY in MainRule (HEADERS|NAME)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}

--- more_headers
FooBar: SomeValue234
--- request
GET /
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=HEADERS|NAME&id0=42424242&var_name0=FooBar, client: 127\.0\.0\.1,@


=== TEST 9.5: ANY in MainRule (URL)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}

--- request
GET /foobar32
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=URL&id0=42424242&var_name0=, client: 127\.0\.0\.1,@


=== TEST 9.6: ANY in MainRule (RAW_BODY)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
    SecRulesEnabled;
    DeniedUrl "/RequestDenied";
    CheckRule "$SQL >= 8" BLOCK;
    CheckRule "$RFI >= 8" BLOCK;
    CheckRule "$TRAVERSAL >= 4" BLOCK;
    CheckRule "$XSS >= 8" BLOCK;
    CheckRule "$TEST >= 8" ALLOW;
    BasicRule wl:11 "mz:BODY|RAW_BODY";

    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;
}
location /RequestDenied {
    return 412;
}

--- raw_request eval
"POST / HTTP/1.1\r
Host: 127.0.0.1\r
Connection: Close\r
User-Agent: Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10\r
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r
Accept-Language: en-us,en;q=0.5\r
Accept-Encoding: gzip, deflate\r
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r
Referer: http://127.0.0.1/\r
Content-Type: application/x-www-form-urlencoded\r
Content-Length: 81\r
\r
txtName=This+is+an+example+of+post+body+with+foobar+in+it.+very+simple,+but+works\r
"
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=BODY&id0=42424242&var_name0=txtName, client: 127\.0\.0\.1,@


=== TEST 9.6: ANY in MainRule (FILE_EXT)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "rx:\.ph|\.asp|\.ht|\.jsp" "mz:ANY" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;
	BasicRule wl:42424242 "mz:RAW_BODY|BODY|URL|HEADERS";
	BasicRule wl:42424242 "mz:HEADERS|NAME";
	BasicRule wl:42424242 "mz:BODY|NAME";

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}

--- raw_request eval
"POST /index.html HTTP/1.1\r
Host: 127.0.0.1\r
Connection: Close\r
User-Agent: Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10\r
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r
Accept-Language: en-us,en;q=0.5\r
Accept-Encoding: gzip, deflate\r
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r
Referer: http://127.0.0.1/index.html\r
Content-Type: multipart/form-data; boundary=---------------------------85477017311078916741744433009\r
Content-Length: 3264\r
\r
-----------------------------85477017311078916741744433009\r
Content-Disposition: form-data; name=\"textline\"\r
\r
valid text bad file\r
-----------------------------85477017311078916741744433009\r
Content-Disposition: form-data; name=\"datafile\"; filename=\"bla_med.php\"\r
Content-Type: application/octet-stream\r
\r
1234567891011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798991001011021031041051061071081091101111121131141151161171181191201211221231241251261271281291301311321331341351361371381391401411421431441451461471481491501511521531541551561571581591601611621631641651661671681691701711721731741751761771781791801811821831841851861871881891901911921931941951961971981992002012022032042052062072082092102112122132142152162172182192202212222232242252262272282292302312322332342352362372382392402412422432442452462472482492502512522532542552562572582592602612622632642652662672682692702712722732742752762772782792802812822832842852862872882892902912922932942952962972982993003013023033043053063073083093103113123133143153163173183193203213223233243253263273283293303313323333343353363373383393403413423433443453463473483493503513523533543553563573583593603613623633643653663673683693703713723733743753763773783793803813823833843853863873883893903913923933943953963973983994004014024034044054064074084094104114124134144154164174184194204214224234244254264274284294304314324334344354364374384394404414424434444454464474484494504514524534544554564574584594604614624634644654664674684694704714724734744754764774784794804814824834844854864874884894904914924934944954964974984995005015025035045055065075085095105115125135145155165175185195205215225235245255265275285295305315325335345355365375385395405415425435445455465475485495505515525535545555565575585595605615625635645655665675685695705715725735745755765775785795805815825835845855865875885895905915925935945955965975985996006016026036046056066076086096106116126136146156166176186196206216226236246256266276286296306316326336346356366376386396406416426436446456466476486496506516526536546556566576586596606616626636646656666676686696706716726736746756766776786796806816826836846856866876886896906916926936946956966976986997007017027037047057067077087097107117127137147157167177187197207217227237247257267277287297307317327337347357367377387397407417427437447457467477487497507517527537547557567577587597607617627637647657667677687697707717727737747757767777787797807817827837847857867877887897907917927937947957967977987998008018028038048058068078088098108118128138148158168178188198208218228238248258268278288298308318328338348358368378388398408418428438448458468478488498508518528538548558568578588598608618628638648658668678688698708718728738748758768778788798808818828838848858868878888898908918928938948958968978988999009019029039049059069079089099109119129139149159169179189199209219229239249259269279289299309319329339349359369379389399409419429439449459469479489499509519529539549559569579589599609619629639649659669679689699709719729739749759769779789799809819829839849859869879889899909919929939949959969979989991000\r
-----------------------------85477017311078916741744433009--\r
"
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=FILE_EXT&id0=42424242&var_name0=datafile, client: 127\.0\.0\.1,@


=== TEST 9.7: ANY in whitelist (URL)
--- user_files
>>> foobar32
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:URL" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;
	BasicRule wl:42424242 "mz:ANY" "msg:ANY matchzone whitelist";

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}

--- request
GET /foobar32
--- error_code: 200


=== TEST 9.8: ANY in whitelist but with URL filter DROP (URL)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:URL" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;
	BasicRule wl:42424242 "mz:$URL:/purple/|ANY" "msg:ANY matchzone whitelist";

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}

--- request
GET /foobar32
--- error_code: 412
--- error_log eval
qr@config=drop&rid=[^&]+&zone0=URL&id0=42424242&var_name0=, client: 127\.0\.0\.1,@


=== TEST 9.9: ANY in whitelist but with URL filter ALLOW (RAW_BODY)
--- user_files
>>> toto
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:RANDOMTHINGS" "mz:RAW_BODY" "s:DROP" id:42424242;
--- config
location / {
    SecRulesEnabled;
    DeniedUrl "/RequestDenied";
    CheckRule "$SQL >= 8" BLOCK;
    CheckRule "$RFI >= 8" BLOCK;
    CheckRule "$TRAVERSAL >= 4" BLOCK;
    CheckRule "$XSS >= 8" BLOCK;
    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;
    error_page 405 = $uri;
    BasicRule wl:11 "mz:$URL:/toto|BODY";
    BasicRule wl:42424242 "mz:$URL:/toto|ANY";
}
location /RequestDenied {
         return 412;
}
--- more_headers
Content-Type: RAFARAFA
--- request eval
use URI::Escape;
"POST /toto

RANDOMTHINGS
"
--- error_code: 200


=== TEST 9.10: ANY in whitelist but with URL filter ALLOW (URL)
--- user_files
>>> foobar32
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
MainRule "str:foobar" "mz:URL" "msg:ANY matchzone" "s:DROP" id:42424242;
--- config
location / {
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
	CheckRule "$TEST >= 8" ALLOW;
	BasicRule wl:42424242 "mz:$URL:/foobar32|ANY" "msg:ANY matchzone whitelist";

  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}
location /RequestDenied {
	return 412;
}

--- request
GET /foobar32
--- error_code: 200


=== TEST 9.11: Main rules and URL ANY whitelist (subpath)
--- user_files
>>> foobar32
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
include $TEST_NGINX_NAXSI_BLOCKING_RULES/*;
--- config
location / {
    include $TEST_NGINX_NAXSI_WHITELISTS_RULES/*;
    SecRulesEnabled;
    DeniedUrl "/RequestDenied";
    CheckRule "$SQL >= 8" BLOCK;
    CheckRule "$RFI >= 8" BLOCK;
    CheckRule "$TRAVERSAL >= 4" BLOCK;
    CheckRule "$XSS >= 8" BLOCK;
    CheckRule "$UWA >= 8" BLOCK;
    BasicRule wl:1000,1011,1013 "mz:$URL:/foobar32|ANY";

    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;
}
location /RequestDenied {
    return 412;
}

--- request
GET /foobar32?id=)union%27select
--- error_code: 200

=== TEST 9.12: Main rules and URL ANY whitelist (root)
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
include $TEST_NGINX_NAXSI_BLOCKING_RULES/*;
--- config
location / {
    include $TEST_NGINX_NAXSI_WHITELISTS_RULES/*;
    SecRulesEnabled;
    DeniedUrl "/RequestDenied";
    CheckRule "$SQL >= 8" BLOCK;
    CheckRule "$RFI >= 8" BLOCK;
    CheckRule "$TRAVERSAL >= 4" BLOCK;
    CheckRule "$XSS >= 8" BLOCK;
    CheckRule "$UWA >= 8" BLOCK;
    BasicRule wl:1000,1011,1013 "mz:$URL:/|ANY";

    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;
}
location /RequestDenied {
    return 412;
}

--- request
GET /?id=)union%27select
--- error_code: 200

