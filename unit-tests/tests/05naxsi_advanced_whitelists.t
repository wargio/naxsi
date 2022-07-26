#vi:filetype=perl

use lib 'lib';
use Test::Nginx::Socket;

plan tests => repeat_each(1) * blocks();
no_root_location();
no_long_string();
$ENV{TEST_NGINX_SERVROOT} = server_root();
run_tests();
__DATA__
=== WL TEST 5.0: Two whitelists on two named arguments, same URL 
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:bla|$URL:/buixor";
	BasicRule wl:1998 "mz:$ARGS_VAR:blu|$URL:/buixor";
}
location /RequestDenied {
	return 412;
}
--- request
GET /buixor?bla=1999
--- error_code: 200
=== WL TEST 5.1: Two whitelists on two named arguments, same URL
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:bla|$URL:/buixor";
	BasicRule wl:1998 "mz:$ARGS_VAR:blu|$URL:/buixor";
}
location /RequestDenied {
	return 412;
}
--- request
GET /buixor?blu=1999
--- error_code: 412
=== WL TEST 5.2: Two whitelists on two named arguments, same URL
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:bla|$URL:/buixor";
	BasicRule wl:1998 "mz:$ARGS_VAR:blu|$URL:/buixor";
}
location /RequestDenied {
	return 412;
}
--- request
GET /buixor?bla=1999&blu=1998
--- error_code: 200
=== WL TEST 5.3: Two whitelists on two named arguments, same URL
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:bla|$URL:/buixor";
	BasicRule wl:1998 "mz:$ARGS_VAR:blu|$URL:/buixor";
}
location /RequestDenied {
	return 412;
}
--- request
GET /?buixor=1998
--- error_code: 412
=== WL TEST 5.4: Whitelists on ARGS/URLs that are URLencoded
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:b_@_la|$URL:/buixor";
	BasicRule wl:1998 "mz:$ARGS_VAR:blu|$URL:/buixor";
}
location /RequestDenied {
	return 412;
}
--- request
GET /buixor?b_@_la=1999
--- error_code: 200

=== WL TEST 5.5: Whitelists on ARGS/URLs that are URLencoded
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:b[]la|$URL:/buixor";
	BasicRule wl:1998 "mz:$ARGS_VAR:blu|$URL:/buixor";
}
location /RequestDenied {
	return 412;
}
--- request
GET /buixor?b]la=1999
--- error_code: 412

=== WL TEST 6: Whitelists trying to provoke collisions ($ARGS_VAR:x + $URL:x|ARGS)
--- user_files
>>> buixor
eh yo
>>> bla
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
#	BasicRule wl:1999 "mz:$ARGS_VAR:/bla";
	BasicRule wl:1998 "mz:$URL:/bla|ARGS";
}	
location /RequestDenied {
	return 412;
}
--- request
GET /bla?1998
--- error_code: 200

=== WL TEST 6.0: Whitelists trying to provoke collisions ($ARGS_VAR:x + $URL:x|ARGS)
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
#	BasicRule wl:1999 "mz:$ARGS_VAR:/bla";
	BasicRule wl:1998 "mz:$URL:/bla|ARGS";
}	
location /RequestDenied {
	return 412;
}
--- request
GET /?/bla=1998
--- error_code: 412

=== WL TEST 6.1: Whitelists trying to provoke collisions ($ARGS_VAR:x + $URL:x|ARGS)
--- user_files
>>> buixor
eh yo
>>> bla
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:bla";
	BasicRule wl:1998 "mz:$URL:/bla|ARGS";
}	
location /RequestDenied {
	return 412;
}
--- request
GET /bla?bla=1999&toto=1998
--- error_code: 200

=== WL TEST 6.2: Whitelists trying to provoke collisions ($ARGS_VAR:x + $URL:x|ARGS)
--- user_files
>>> buixor
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:/bla";
	BasicRule wl:1998 "mz:$URL:/bla|ARGS";
}	
location /RequestDenied {
	return 412;
}
--- request
GET /buixor?/bla=1999
--- error_code: 200

=== WL TEST 6.3: Whitelists trying to provoke collisions ($ARGS_VAR:x + $URL:x|ARGS)
--- user_files
>>> buixor
eh yo
>>> bla
eh yo
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule "str:1998" "msg:foobar test pattern" "mz:ARGS" "s:$SQL:42" id:1998;
MainRule "str:1999" "msg:foobar test pattern #2" "mz:ARGS" "s:$SQL:42" id:1999;
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
	BasicRule wl:1999 "mz:$ARGS_VAR:/bla";
	BasicRule wl:1998 "mz:$URL:/bla|ARGS";
}	
location /RequestDenied {
	return 412;
}
--- request
GET /bla?/bla=1999&bu=1998
--- error_code: 200


=== WL TEST 7.0: Whitelists robots.txt as MainRule
--- user_files
>>> robots.txt
aaaa
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:20000003 "s:$UWA:8" "rx:\.txt$" "mz:URL" "msg:file access to .txt";
MainRule wl:20000003 "mz:$URL:/robots.txt|URL";
--- config
location / {
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}	
location /RequestDenied {
	return 412;
}
--- request
GET /robots.txt
--- error_code: 200


=== WL TEST 7.1: Whitelists robots.txt as BasicRule
--- user_files
>>> robots.txt
aaaa
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
MainRule id:20000003 "s:$UWA:8" "rx:\.txt$" "mz:URL" "msg:file access to .txt";
--- config
location / {
	BasicRule wl:20000003 "mz:$URL:/robots.txt|URL";
	#LearningMode;
	SecRulesEnabled;
	DeniedUrl "/RequestDenied";
	CheckRule "$SQL >= 8" BLOCK;
	CheckRule "$RFI >= 8" BLOCK;
	CheckRule "$TRAVERSAL >= 4" BLOCK;
	CheckRule "$XSS >= 8" BLOCK;
  	root $TEST_NGINX_SERVROOT/html/;
	index index.html index.htm;
}	
location /RequestDenied {
	return 412;
}
--- request
GET /robots.txt
--- error_code: 200

