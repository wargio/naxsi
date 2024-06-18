### Introduction

[Fail2ban]( http://fail2ban.org ) is a nice piece of software allowing you to act on the IP address of someone abusing you, usually banning him using [netfilter]( http://netfilter.org ). Basically you want to ban all skids bruteforcing you SSH service or your webmail login form.

We figured out that *dropping skids^Whackers is cool, banning them is even better*. So this howto will show you how to ban those who are appearing to much in your naxsi logs.

# Requirements
* An OS running Fail2ban and Naxsi
* A few minutes

# Configuration
Very simple, create `/etc/fail2ban/filter.d/nginx-naxsi.conf` with:

```ini
[INCLUDES]
before = common.conf
[Definition]
failregex = NAXSI_FMT: ip=<HOST>&server=.*&uri=.*&learning=0
            NAXSI_FMT: ip=<HOST>.*&config=block
ignoreregex = NAXSI_FMT: ip=<HOST>.*&config=learning
```

And add a section within `/etc/fail2ban/jail.conf` with:

```ini
[nginx-naxsi]
enabled = true
port = http,https
filter = nginx-naxsi
logpath = /var/log/nginx/*error.log
maxretry = 6
```

So in `/var/log/fail2ban.log`, any time the same IP triggers Naxsi 6 times in 5 minutes (`fail2ban findtime=600`) you should see:

```
2012-06-29 15:34:44,016 fail2ban.actions: WARNING [nginx-naxsi] Ban 88.z.x.y`
```

BONUS: Graph this new fail2ban jail with [Munin]( http://munin-monitoring.org/ ) so you can track how many guys are trying to abuse your website :)