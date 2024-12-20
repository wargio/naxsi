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

=== WL TEST 8: Whitelists inheritance
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
    location /child/     {
	return 200 "200 $request_uri";
    }
    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;
}	
location /RequestDenied {
    return 412;
}
--- request
GET /child/robots.txt
--- error_code: 200


=== WL TEST 9: FILE_EXT can be mixed with URL_X
--- main_config
working_directory /tmp/;
worker_rlimit_core 25M;
--- main_config
load_module $TEST_NGINX_NAXSI_MODULE_SO;
--- http_config
include $TEST_NGINX_NAXSI_RULES;
--- config
location / {
    BasicRule wl:1500 "mz:$URL_X:^/submit/\d+/feedback$|FILE_EXT";

    #LearningMode;
    SecRulesEnabled;
    DeniedUrl "/RequestDenied";
    CheckRule "$SQL >= 8" BLOCK;
    CheckRule "$RFI >= 8" BLOCK;
    CheckRule "$TRAVERSAL >= 4" BLOCK;
    CheckRule "$UPLOAD >= 8" BLOCK;
    CheckRule "$XSS >= 8" BLOCK;
    root $TEST_NGINX_SERVROOT/html/;
    index index.html index.htm;

}
location /RequestDenied {
    return 412;
}
--- raw_request eval
"POST /submit/875411/feedback HTTP/1.1\r
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
Content-Disposition: form-data; name=\"datafile\"; filename=\"bla_med.phx\"\r
Content-Type: application/octet-stream\r
\r
1234567891011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859606162636465666768697071727374757677787980818283848586878889909192939495969798991001011021031041051061071081091101111121131141151161171181191201211221231241251261271281291301311321331341351361371381391401411421431441451461471481491501511521531541551561571581591601611621631641651661671681691701711721731741751761771781791801811821831841851861871881891901911921931941951961971981992002012022032042052062072082092102112122132142152162172182192202212222232242252262272282292302312322332342352362372382392402412422432442452462472482492502512522532542552562572582592602612622632642652662672682692702712722732742752762772782792802812822832842852862872882892902912922932942952962972982993003013023033043053063073083093103113123133143153163173183193203213223233243253263273283293303313323333343353363373383393403413423433443453463473483493503513523533543553563573583593603613623633643653663673683693703713723733743753763773783793803813823833843853863873883893903913923933943953963973983994004014024034044054064074084094104114124134144154164174184194204214224234244254264274284294304314324334344354364374384394404414424434444454464474484494504514524534544554564574584594604614624634644654664674684694704714724734744754764774784794804814824834844854864874884894904914924934944954964974984995005015025035045055065075085095105115125135145155165175185195205215225235245255265275285295305315325335345355365375385395405415425435445455465475485495505515525535545555565575585595605615625635645655665675685695705715725735745755765775785795805815825835845855865875885895905915925935945955965975985996006016026036046056066076086096106116126136146156166176186196206216226236246256266276286296306316326336346356366376386396406416426436446456466476486496506516526536546556566576586596606616626636646656666676686696706716726736746756766776786796806816826836846856866876886896906916926936946956966976986997007017027037047057067077087097107117127137147157167177187197207217227237247257267277287297307317327337347357367377387397407417427437447457467477487497507517527537547557567577587597607617627637647657667677687697707717727737747757767777787797807817827837847857867877887897907917927937947957967977987998008018028038048058068078088098108118128138148158168178188198208218228238248258268278288298308318328338348358368378388398408418428438448458468478488498508518528538548558568578588598608618628638648658668678688698708718728738748758768778788798808818828838848858868878888898908918928938948958968978988999009019029039049059069079089099109119129139149159169179189199209219229239249259269279289299309319329339349359369379389399409419429439449459469479489499509519529539549559569579589599609619629639649659669679689699709719729739749759769779789799809819829839849859869879889899909919929939949959969979989991000\r
-----------------------------85477017311078916741744433009--\r
"
--- error_code: 404

